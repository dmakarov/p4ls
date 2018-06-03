#include "p4unit.h"

#include <frontends/p4/frontend.h>
#include <frontends/common/parseInput.h>
#include <lib/error.h>

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <string>


AST::AST()
{
}

P4_file::P4_file(const std::string &command, const std::string &path, const std::string &text)
	: _p4(new P4CContextWithOptions<p4options>)
	, _options(P4CContextWithOptions<p4options>::get().options())
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
			auto temp = boost::filesystem::unique_path();
			_argv[ii] = new char[temp.native().size() + 1];
			strncpy(_argv[ii], temp.native().c_str(), temp.native().size());
			std::ofstream ofs(temp.native());
			ofs << text;
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Wrote document " << path << " text to a temporary file " << temp.native();
#endif
		}
		else
		{
			_argv[ii] = new char[it->size() + 1];
			strncpy(_argv[ii], it->c_str(), it->size());
		}
	}
	auto *remaining_options = _options.process(_argc, _argv);
	_options.setInputFile();
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Input file " << _options.file;
#endif
	_options.langVersion = CompilerOptions::FrontendVersion::P4_16;
#if 0
	_program.reset(P4::parseP4File(_options));
#endif
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Compiled p4 source file, number of errors " << ::errorCount();
#endif
}

P4_file::~P4_file()
{
	if (_argv)
	{
		for (auto ii = 0; ii < _argc; ++ii)
			delete [] _argv[ii];
		delete [] _argv;
	}
}
