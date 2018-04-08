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
	using handler_type = std::function<void (const rapidjson::Document&)>;

	Dispatcher() = default;

	void register_handler(const std::string &method, handler_type handler);

	bool call(rapidjson::Document &msg) const;

private:

	std::unordered_map<std::string, handler_type> handlers;
};
