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

	Dispatcher(handler_type error_handler) : _error_handler(error_handler) {}
	void register_handler(const std::string &method, handler_type handler);
	bool call(rapidjson::Document &msg, std::ostream &output_stream) const;

private:

	std::unordered_map<std::string, handler_type> _handlers;
	handler_type _error_handler;
};

void reply(rapidjson::Value &result);
void reply(const std::string &msg);
