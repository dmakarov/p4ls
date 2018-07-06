/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"

#include <boost/log/common.hpp>

#include <rapidjson/document.h>

#include <functional>
#include <string>
#include <unordered_map>


class Dispatcher {
public:
	using handler_type = std::function<void (const rapidjson::Value&)>;
	static boost::log::sources::severity_logger<int> _logger;

	Dispatcher(handler_type error_handler) : _error_handler(error_handler) {}
	void register_handler(const std::string &method, handler_type handler);
	void call(std::string content, std::ostream &output_stream) const;

private:
	static const std::string _JSONRPC_VERSION;
	std::unordered_map<std::string, handler_type> _handlers;
	handler_type _error_handler;
};

void register_protocol_handlers(Dispatcher &dispatcher, Protocol &protocol);

void reply(rapidjson::Value &result);
void reply(ERROR_CODES code, const std::string &msg);
