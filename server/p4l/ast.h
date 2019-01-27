/*
 * -*- c++ -*-
 */

#pragma once

#include <istream>
#include <string>
#include <vector>

#include "lexer.h"

namespace p4l {

class AST {
 public:
	virtual ~AST() {}
};

class ExprAST : public AST {
 public:
	virtual ~ExprAST() {}
};

class NumberExprAST : public ExprAST {
 public:
	explicit NumberExprAST(int value) : value(value) {
	}

 private:
	int value = 0;
	int width = 32;
};

class VariableExprAST : public ExprAST {
 public:
	explicit VariableExprAST(const std::string& name) : name(name) {
	}

 private:
	std::string name;
};

class BinaryExprAST : public ExprAST {
 public:
	BinaryExprAST(Token op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
		: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {
	}

 private:
	Token op;
	std::unique_ptr<ExprAST> lhs, rhs;
};

class PrototypeAST : public AST {
 public:
	PrototypeAST(const std::string& name, std::vector<std::string> args)
		: name(name), args(std::move(args)) {
	}

	const std::string& get_name() const {
		return name;
	}

 private:
	std::string name;
	std::vector<std::string> args;
};

class FunctionAST : public AST {
 public:
	FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
		: proto(std::move(proto)), body(std::move(body)) {
	}

 private:
	std::unique_ptr<PrototypeAST> proto;
	std::unique_ptr<ExprAST> body;
};

} // namespace p4l
