#include "p4unit.h"

#include <frontends/p4/frontend.h>
#include <frontends/common/parseInput.h>
#include <lib/error.h>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <sstream>


P4_file::P4_file(const std::string &command, const std::string &path, const std::string &text)
	: _p4(new P4CContextWithOptions<p4options>)
	, _options(P4CContextWithOptions<p4options>::get().options())
	, _temp_path(boost::filesystem::unique_path().native())
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "P4_file constructor started";
#endif
	boost::char_separator<char> separator(" ");
	boost::tokenizer<boost::char_separator<char>> tokens(command, separator);
	_argc = 0;
	for (auto it = tokens.begin(); it != tokens.end(); ++it) ++_argc;
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Command split in " << _argc << " tokens.";
#endif
	_argv = new char*[_argc];
	int ii = 0;
	for (auto it = tokens.begin(); it != tokens.end(); ++it, ++ii)
	{
#if LOGGING_ENABLED
		BOOST_LOG_TRIVIAL(info) << "Token " << ii << " " << *it;
#endif
		if (*it == path)
		{
			_argv[ii] = new char[_temp_path.size() + 1];
			strncpy(_argv[ii], _temp_path.c_str(), _temp_path.size() + 1);
			std::ofstream ofs(_temp_path);
			ofs << text;
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Wrote document " << path << " text to a temporary file " << _temp_path;
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
	BOOST_LOG_TRIVIAL(info) << "Processed options, number of errors " << ::errorCount() << " input file " << _options.file;
#endif
	_program.reset(P4::parseP4File(_options));
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Compiled p4 source file, number of errors " << ::errorCount();
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

void P4_file::get_symbols(std::vector<Symbol_information>& symbols)
{
	Outline outline(_options, symbols);
	outline.process(_program);
}
