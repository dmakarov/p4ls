/*
 * -*- c++ -*-
 */

#pragma once

#include <frontends/common/options.h>
#include <ir/ir.h>

#include <boost/optional.hpp>

#include <memory>

/**
 *
 */
class AST {

public:
	AST();

};

class p4options : public CompilerOptions {};

/**
 *
 */
class P4_file {

public:
	P4_file(const std::string &command, const std::string &path, const std::string &text);
	~P4_file();

private:
	int _argc;
	char **_argv;
	AutoCompileContext _p4;
	p4options &_options;
	boost::optional<AST> _ast;
	std::unique_ptr<const IR::P4Program> _program;
};
