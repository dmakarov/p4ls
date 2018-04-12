/*
 * -*- c++ -*-
 */

#pragma once

#include <rapidjson/document.h>

#include <functional>
#include <string>
#include <unordered_map>


class Dispatcher {
public:
	using handler_type = std::function<void (const rapidjson::Value&)>;

	Dispatcher() = default;

	void register_handler(const std::string &method, handler_type handler);

	bool call(rapidjson::Document &msg, std::ostream &output_stream) const;

private:

	std::unordered_map<std::string, handler_type> _handlers;
};

void reply(rapidjson::Value &result);
