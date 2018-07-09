#include "p4unit.h"

#include <frontends/p4/frontend.h>
#include <frontends/common/parseInput.h>
#include <lib/error.h>

#include <boost/log/attributes/constant.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <sstream>

namespace {
boost::log::sources::severity_logger<int> _logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

std::string::size_type get_position_index(const std::string& content, const Position& position)
{
	const std::string newline(1, '\n');
	std::string::size_type index = 0;
	for (auto it = 0U; it != position._line; ++it)
	{
		auto next_index = content.find_first_of(newline, index);
		if (next_index == std::string::npos)
		{
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "has " << it + 1 << " lines, but changes requested on line " << position._line;
			return std::string::npos;
		}
		index = next_index + 1;
	}
	auto line_end_index = content.find_first_of(newline, index);
	if (line_end_index == std::string::npos)
	{
		line_end_index = content.size();
	}
	if (index + position._character < line_end_index)
	{
		return index + position._character;
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "line " << position._line << " is shorter than " << position._character << " characters";
	return std::string::npos;
}

}

bool Symbol_collector::preorder(const IR::Node* node)
{
	if (!node->is<Util::IHasSourceInfo>())
	{
		return false;
	}
	if (auto ctxt = getContext())
	{
		Range range;
		const char* file = "";
		if (auto info = node->getSourceInfo())
		{
			range.set(info);
			file = info.getSourceFile().c_str();
		}
		auto* path = _temp_path == file ? _unit_path.c_str() : file;
		Location location{path, range};
		BOOST_LOG(_logger)
			<< ctxt->depth
			<< " " << node->node_type_name()
			<< " " << node->toString()
			<< " " << location;
		if (node->is<IR::Type_Header>())
		{
			std::ostringstream definition;
			definition << node->toString() << " {\n";
			for (auto it : node->to<IR::Type_Header>()->fields)
			{
				definition << "  " << it->type->toString() << " " << it->toString() << ";\n";
			}
			definition << "}";
			auto* name = node->to<IR::IDeclaration>()->getName().toString().c_str();
			_definitions.emplace(name, definition.str());
			BOOST_LOG(_logger) << definition.str();
		}
		if (node->is<IR::Type_Name>())
		{
			_locations[path].emplace(std::make_pair(range, node->toString().c_str()));
		}
		else if (ctxt->depth == _max_depth
			&& node->is<IR::IDeclaration>()
			&& !node->is<IR::Type_Control>()
			&& !node->is<IR::Type_Parser>())
		{
			boost::optional<std::string> container;
			if (!_container.empty())
			{
				container.emplace(_container.back());
			}
			auto* name = node->to<IR::IDeclaration>()->getName().toString().c_str();
			_symbols.emplace_back(name, get_symbol_kind(node), location, container);
			if (node->is<IR::Type_Header>()
				|| node->is<IR::Type_Struct>()
				|| node->is<IR::P4Control>()
				|| node->is<IR::P4Parser>())
			{
				_container.push_back(name);
				++_max_depth;
			}
		}
	}
	return true;
}

void Symbol_collector::postorder(const IR::Node* node)
{
	if (auto ctxt = getContext())
	{
		BOOST_LOG(_logger) << "exit from " << ctxt->depth << " " << node->node_type_name();
		if (ctxt->depth < _max_depth)
		{
			--_max_depth;
			_container.pop_back();
		}
	}
}

SYMBOL_KIND Symbol_collector::get_symbol_kind(const IR::Node* node)
{
	if (node->is<IR::Declaration_Constant>())
	{
		return SYMBOL_KIND::Constant;
	}
	if (node->is<IR::Type_Header>() ||
		node->is<IR::Type_Struct>())
	{
		return SYMBOL_KIND::Class;
	}
	if (node->is<IR::P4Parser>() ||
		node->is<IR::P4Control>())
	{
		return SYMBOL_KIND::Class;
	}
	if (node->is<IR::Type_Typedef>())
	{
		return SYMBOL_KIND::Interface;
	}
	if (node->is<IR::Declaration_Instance>())
	{
		return SYMBOL_KIND::Variable;
	}
	if (node->is<IR::StructField>())
	{
		return SYMBOL_KIND::Field;
	}
	if (node->is<IR::ParserState>() ||
		node->is<IR::P4Action>() ||
		node->is<IR::P4Table>())
	{
		return SYMBOL_KIND::Method;
	}
	return SYMBOL_KIND::Null;
}

P4_file::P4_file(const std::string &command, const std::string &unit_path, const std::string& text)
	: _command(std::make_unique<char[]>(command.size() + 1))
	, _unit_path(unit_path)
	, _source_code(text)
	, _changed(true)
{
	_logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("P4UNIT"));
	BOOST_LOG(_logger) << "constructor started.";
	boost::char_separator<char> separator(" ");
	boost::tokenizer<boost::char_separator<char>> tokens(command, separator);
	auto arg = _command.get();
	for (auto it = tokens.begin(); it != tokens.end(); ++it)
	{
		BOOST_LOG(_logger) << "process token " << *it;
		auto size = it->size() + 1;
		strncpy(arg, it->c_str(), size);
		_argv.emplace_back(arg);
		arg += size;
	}
	compile();
	BOOST_LOG(_logger) << "constructed.";
}

void P4_file::change_source_code(const std::vector<Text_document_content_change_event>& content_changes)
{
	_changed = true;
	for (auto& it : content_changes)
	{
		if (!it._range)
		{
			_source_code = it._text;
			BOOST_LOG(_logger) << "replaced entire source code with new content.";
		}
		else
		{
			const auto start = get_position_index(_source_code, it._range->_start);
			const auto end = get_position_index(_source_code, it._range->_end);
			if (start != std::string::npos && end != std::string::npos &&
				start <= end && (!it._range_length || *it._range_length == end - start))
			{
				std::string content;
				content.reserve(start + it._text.size() + _source_code.size() - end);
				content = _source_code.substr(0, start);
				content += it._text;
				content += _source_code.substr(end);
				_source_code = std::move(content);
				BOOST_LOG(_logger) << "applied content change in range " << *it._range;
			}
			else
			{
				BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "ignore invalid content change in range " << *it._range;
			}
		}
	}
}

std::vector<Symbol_information>& P4_file::get_symbols()
{
	if (_changed)
	{
		compile();
	}
	return _symbols;
}

boost::optional<std::string> P4_file::get_hover(const Location& location)
{
	BOOST_LOG(_logger) << "search hover for " << location;
	for (const auto& it : _locations[location._uri])
	{
	BOOST_LOG(_logger) << "check location " << it.first;
		if (it.first & location._range)
		{
			return _definitions[it.second];
		}
	}
	return boost::none;
}

void P4_file::compile()
{
	AutoCompileContext p4c_context(new P4CContextWithOptions<p4options>);
	p4options& p4c_options(P4CContextWithOptions<p4options>::get().options());
	p4c_options.langVersion = CompilerOptions::FrontendVersion::P4_16;
	p4c_options.process(_argv.size(), _argv.data());
	BOOST_LOG(_logger) << "processed options, number of errors " << ::errorCount();
	auto temp_file_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.p4");
	p4c_options.file = temp_file_path.native();
	std::ofstream ofs(p4c_options.file);
	ofs << _source_code;
	ofs.close();
	BOOST_LOG(_logger) << "wrote document \"" << _unit_path << "\" to a temporary file \"" << p4c_options.file << "\"";
	_program.reset(P4::parseP4File(p4c_options));
	auto error_count = ::errorCount();
	BOOST_LOG(_logger) << "compiled p4 source file, number of errors " << error_count;
	auto existed = remove(temp_file_path);
	BOOST_LOG(_logger) << "removed temporary file " << temp_file_path << " " << existed;
	if (_program && error_count == 0)
	{
		_symbols.clear();
		_definitions.clear();
		_locations.clear();
		Collected_data output{_symbols, _definitions, _locations};
		Outline outline(p4c_options, _unit_path, output);
		outline.process(_program);
		_changed = false;
	}
}
