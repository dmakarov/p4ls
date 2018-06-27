/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"

#include <frontends/common/options.h>
#include <ir/ir.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <map>
#include <memory>
#include <string>
#include <unordered_map>


class p4options : public CompilerOptions {
};


struct Collected_data {
	Collected_data(std::vector<Symbol_information>& symbols,
				   std::unordered_map<std::string, std::string>& definitions,
				   std::unordered_map<std::string, std::map<Range, std::string>>& locations)
		: _symbols(symbols)
		, _definitions(definitions)
		, _locations(locations)
	{}
	std::vector<Symbol_information>& _symbols;
	std::unordered_map<std::string, std::string>& _definitions;
	std::unordered_map<std::string, std::map<Range, std::string>>& _locations;
};


class Symbol_collector : public Inspector {
public:
	Symbol_collector(cstring temp_path, std::string& unit_path, Collected_data& output)
		: _temp_path(temp_path)
		, _unit_path(unit_path)
		, _max_depth(1)
		, _symbols(output._symbols)
		, _definitions(output._definitions)
		, _locations(output._locations)
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
	std::unordered_map<std::string, std::string>& _definitions;
	std::unordered_map<std::string, std::map<Range, std::string>>& _locations;
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
	P4_file() : _p4(new P4CContextWithOptions<p4options>)
			  , _options(P4CContextWithOptions<p4options>::get().options())
	{}
	P4_file(const std::string &command, const std::string &unit_path);
	~P4_file() = default;
	void compile(const std::string& text);
	std::vector<Symbol_information>& get_symbols();
	boost::optional<std::string> get_hover(const Location& location);

private:
	AutoCompileContext _p4;
	p4options& _options;
	std::unique_ptr<const IR::P4Program> _program;
	std::string _unit_path;
	std::vector<Symbol_information> _symbols;
	std::unordered_map<std::string, std::string> _definitions;
	std::unordered_map<std::string, std::map<Range, std::string>> _locations;
};
