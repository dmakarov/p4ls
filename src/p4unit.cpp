#include "p4unit.h"

#include <frontends/p4/frontend.h>
#include <frontends/common/parseInput.h>
#include <lib/error.h>

#include <boost/filesystem.hpp>
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
		if (node->is<IR::Type_Name>())
		{
			_location_to_name[path].emplace(std::make_pair(range, node->toString().c_str()));
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

P4_file::P4_file(const std::string &command, const std::string &unit_path, const std::string &text)
	: _p4(new P4CContextWithOptions<p4options>)
	, _options(P4CContextWithOptions<p4options>::get().options())
	, _temp_path(boost::filesystem::unique_path().native())
	, _unit_path(unit_path)
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "P4_file constructor started";
#endif
	boost::char_separator<char> separator(" ");
	boost::tokenizer<boost::char_separator<char>> tokens(command, separator);
	_argc = 0;
	for (auto it = tokens.begin(); it != tokens.end(); ++it) ++_argc;
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Command split in " << _argc << " tokens.";
#endif
	_argv = new char*[_argc];
	int ii = 0;
	for (auto it = tokens.begin(); it != tokens.end(); ++it, ++ii)
	{
#if LOGGING_ENABLED
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Token " << ii << " " << *it;
#endif
		if (*it == _unit_path)
		{
			_argv[ii] = new char[_temp_path.size() + 1];
			strncpy(_argv[ii], _temp_path.c_str(), _temp_path.size() + 1);
			std::ofstream ofs(_temp_path);
			ofs << text;
#if LOGGING_ENABLED
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Wrote document " << _unit_path << " text to a temporary file " << _temp_path;
#endif
		}
		else
		{
			_argv[ii] = new char[it->size() + 1];
			strncpy(_argv[ii], it->c_str(), it->size() + 1);
		}
	}
	_options.langVersion = CompilerOptions::FrontendVersion::P4_16;
	_options.process(_argc, _argv);
	_options.setInputFile();
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Processed options, number of errors " << ::errorCount() << " input file " << _options.file;
#endif
	_program.reset(P4::parseP4File(_options));
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Compiled p4 source file, number of errors " << ::errorCount();
#endif
}

P4_file::~P4_file()
{
	if (!_temp_path.empty())
	{
		unlink(_temp_path.c_str());
	}
	if (_argv)
	{
		for (auto ii = 0; ii < _argc; ++ii)
			delete [] _argv[ii];
		delete [] _argv;
	}
}

std::vector<Symbol_information>& P4_file::get_symbols()
{
	_symbols.clear();
	Collected_data output{_symbols, _location_to_name};
	Outline outline(_options, _unit_path, output);
	outline.process(_program);
	return _symbols;
}

boost::optional<std::string> P4_file::get_hover(const Location& location)
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Searching hover for " << location;
#endif
	for (const auto& it : _location_to_name[location._uri])
	{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "Checking location " << it.first;
#endif
		if (it.first & location._range)
		{
			return it.second;
		}
	}
	return boost::none;
}
