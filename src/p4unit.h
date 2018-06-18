/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"

#include <frontends/common/options.h>
#include <ir/ir.h>

#include <boost/log/trivial.hpp>
#include <boost/optional.hpp>

#include <memory>
#include <string>


class p4options : public CompilerOptions {};


class Collector : public Inspector {
public:
	Collector(cstring source_file, std::vector<Symbol_information>& symbols)
		: _source_file(source_file)
		, _symbols(symbols)
	{
		setName("Collector");
	}

	bool preorder(const IR::Node* node) override
	{
		if (auto ctxt = getContext())
		{
			if (ctxt->depth > 1)
			{
				return false;
			}
			if (node->is<IR::IDeclaration>())
			{
				auto si = node->getSourceInfo();
				if (_source_file == si.getSourceFile())
				{
					_symbols.emplace_back(node->to<IR::IDeclaration>()->getName().toString().c_str(), get_symbol_kind(node), Location{_source_file.c_str(), Range{Position{si.getStart().getLineNumber(), si.getStart().getColumnNumber()}, Position{si.getEnd().getLineNumber(), si.getEnd().getColumnNumber()}}}, "");
					BOOST_LOG_TRIVIAL(info) << node->node_type_name() << " " << node->to<IR::IDeclaration>()->getName().toString();
				}
			}
		}
		return true;
	}
private:
	SYMBOL_KIND get_symbol_kind(const IR::Node* node)
	{
		if (node->node_type_name() == "Declaration_Constant")
		{
			return SYMBOL_KIND::Constant;
		}
		if (node->node_type_name() == "Type_Header" ||
			node->node_type_name() == "Type_Struct")
		{
			return SYMBOL_KIND::Class;
		}
		if (node->node_type_name() == "P4Parser" ||
			node->node_type_name() == "P4Control")
		{
			return SYMBOL_KIND::Class;
		}
		if (node->node_type_name() == "Type_Typedef")
		{
			return SYMBOL_KIND::Interface;
		}
		if (node->node_type_name() == "Declaration_Instance")
		{
			return SYMBOL_KIND::Function;
		}
		return SYMBOL_KIND::Null;
	}
	cstring _source_file;
	std::vector<Symbol_information>& _symbols;
};


class Outline : public PassManager {
public:
	explicit Outline(CompilerOptions& options, std::vector<Symbol_information>& symbols)
	{
		setName("Outline");
		addPasses({new Collector(options.file, symbols)});
	}

	void process(std::unique_ptr<const IR::P4Program>& program)
	{
		program->apply(*this);
	}
};


/**
 *
 */
class P4_file {

public:
	P4_file(const std::string &command, const std::string &path, const std::string &text);
	~P4_file();
	void get_symbols(std::vector<Symbol_information>& symbols);

private:
	int _argc;
	char **_argv;
	AutoCompileContext _p4;
	p4options &_options;
	std::unique_ptr<const IR::P4Program> _program;
	std::string _temp_path;
};
