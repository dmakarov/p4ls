/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"

#include <frontends/common/options.h>
#include <ir/ir.h>

#include <boost/optional.hpp>

#include <map>
#include <memory>
#include <string>
#include <unordered_map>


class p4options : public CompilerOptions {
};


struct Collected_data {
	Collected_data(std::vector<Symbol_information>& symbols, std::unordered_map<std::string, std::map<Range, std::string>>& location_to_name)
		: _symbols(symbols)
		, _location_to_name(location_to_name)
	{}
	std::vector<Symbol_information>& _symbols;
	std::unordered_map<std::string, std::map<Range, std::string>>& _location_to_name;
};


class Symbol_collector : public Inspector {
public:
	Symbol_collector(cstring temp_path, std::string& unit_path, Collected_data& output)
		: _temp_path(temp_path)
		, _unit_path(unit_path)
		, _max_depth(1)
		, _symbols(output._symbols)
		, _location_to_name(output._location_to_name)
	{
		setName("Symbol_collector");
	}

	bool preorder(const IR::Node* node) override;
	void postorder(const IR::Node* node) override;

private:
	SYMBOL_KIND get_symbol_kind(const IR::Node* node);

	cstring _temp_path;
	std::string _unit_path;
	int _max_depth;
	std::vector<std::string> _container;
	std::vector<Symbol_information>& _symbols;
	std::unordered_map<std::string, std::map<Range, std::string>>& _location_to_name;
};


class Outline : public PassManager {
public:
	Outline(CompilerOptions& options, std::string& unit_path, Collected_data& output)
	{
		setName("Outline");
		addPasses({new Symbol_collector(options.file, unit_path, output)});
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
	P4_file()
	: _p4(new P4CContextWithOptions<p4options>)
	, _options(P4CContextWithOptions<p4options>::get().options())
	{}
	P4_file(const std::string &command, const std::string &unit_path, const std::string &text);
	~P4_file();
	std::vector<Symbol_information>& get_symbols();
	boost::optional<std::string> get_hover(const Location& location);

private:
	int _argc;
	char **_argv;
	AutoCompileContext _p4;
	p4options &_options;
	std::unique_ptr<const IR::P4Program> _program;
	std::string _temp_path;
	std::string _unit_path;
	std::vector<Symbol_information> _symbols;
	std::unordered_map<std::string, std::map<Range, std::string>> _location_to_name;
};
