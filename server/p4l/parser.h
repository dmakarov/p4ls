/*
 * -*- c++ -*-
 */

#pragma once

#include <istream>
#include <string>

#include "ast.h"
#include "lexer.h"

namespace p4l {

class Parser final {
 public:
	explicit Parser(std::istream& is) : lexer(is) {}
	void run();

 private:
	Token token = Token::START;
	Lexer lexer;
};

} // namespace p4l
