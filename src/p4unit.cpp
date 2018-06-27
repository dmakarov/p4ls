#include "p4unit.h"

#include <frontends/p4/frontend.h>
#include <frontends/common/parseInput.h>
#include <lib/error.h>

#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <sstream>

namespace {
boost::log::sources::severity_logger<int> _logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
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
#if LOGGING_ENABLED
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug)
			<< ctxt->depth
			<< " " << node->node_type_name()
			<< " " << node->toString()
			<< " " << location;
#endif
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
#if LOGGING_ENABLED
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << definition.str();
#endif
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
#if LOGGING_ENABLED
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Exit from " << ctxt->depth << " " << node->node_type_name();
#endif
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

P4_file::P4_file(const std::string &command, const std::string &unit_path)
	: _p4(new P4CContextWithOptions<p4options>)
	, _options(P4CContextWithOptions<p4options>::get().options())
	, _unit_path(unit_path)
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "P4_file constructor started";
#endif
	auto temp_file_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.p4");
	boost::char_separator<char> separator(" ");
	boost::tokenizer<boost::char_separator<char>> tokens(command, separator);
	std::vector<char*> argv;
	for (auto it = tokens.begin(); it != tokens.end(); ++it)
	{
#if LOGGING_ENABLED
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Token " << *it;
#endif
		if (*it == _unit_path)
		{
			auto temp = temp_file_path.native();
			auto arg = new char[temp.size() + 1];
			strncpy(arg, temp.c_str(), temp.size() + 1);
			argv.emplace_back(arg);
		}
		else
		{
			auto arg = new char[it->size() + 1];
			strncpy(arg, it->c_str(), it->size() + 1);
			argv.emplace_back(arg);
		}
	}
	_options.langVersion = CompilerOptions::FrontendVersion::P4_16;
	_options.process(argv.size(), argv.data());
	_options.setInputFile();
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Processed options, number of errors " << ::errorCount() << " input file " << _options.file;
#endif
}

void P4_file::compile(const std::string& text)
{
	std::ofstream ofs(_options.file);
	ofs << text;
	ofs.close();
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Wrote document " << _unit_path << " text to a temporary file " << _options.file;
#endif
	_program.reset(P4::parseP4File(_options));
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Compiled p4 source file, number of errors " << ::errorCount();
#endif
	boost::filesystem::path temp_file_path(_options.file);
	if (exists(temp_file_path))
	{
		remove(temp_file_path);
	}
	_symbols.clear();
	_definitions.clear();
	_locations.clear();
	Collected_data output{_symbols, _definitions, _locations};
	Outline outline(_options, _unit_path, output);
	outline.process(_program);
}

std::vector<Symbol_information>& P4_file::get_symbols()
{
	return _symbols;
}

boost::optional<std::string> P4_file::get_hover(const Location& location)
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Searching hover for " << location;
#endif
	for (const auto& it : _locations[location._uri])
	{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Checking location " << it.first;
#endif
		if (it.first & location._range)
		{
			return _definitions[it.second];
		}
	}
	return boost::none;
}
