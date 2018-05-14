#include "dispatcher.h"
#include "context.h"

#include <boost/log/common.hpp>
#include <boost/optional.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

boost::log::sources::severity_logger<int> Dispatcher::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

namespace {

static Key<int> request_id;
static Key<std::ostream *> request_output_stream;

struct registration_helper {
	template <typename param> void operator()(const std::string &method, void(Protocol::*handler)(param))
	{
		auto *protocol = this->protocol;
		dispatcher.register_handler(method, [=](const rapidjson::Value &json) {
			typename std::remove_reference<param>::type params;
			if (set_params_from_json(json, params))
			{
				(protocol->*handler)(params);
			}
			else
			{
				BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::error) << method << " cannot be handled because setting params failed.";
			}
		});
	}

	Dispatcher &dispatcher;
	Protocol *protocol;
};
} // namespace

void register_protocol_handlers(Dispatcher &dispatcher, Protocol &protocol)
{
	registration_helper register_handler{dispatcher, &protocol};
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	register_handler("exit", &Protocol::on_exit);
	register_handler("initialize", &Protocol::on_initialize);
	register_handler("shutdown", &Protocol::on_shutdown);
	register_handler("textDocument/codeAction", &Protocol::on_textDocument_codeAction);
	register_handler("textDocument/completion", &Protocol::on_textDocument_completion);
	register_handler("textDocument/definition", &Protocol::on_textDocument_definition);
	register_handler("textDocument/didChange", &Protocol::on_textDocument_didChange);
	register_handler("textDocument/didClose", &Protocol::on_textDocument_didClose);
	register_handler("textDocument/didOpen", &Protocol::on_textDocument_didOpen);
	register_handler("textDocument/documentHighlight", &Protocol::on_textDocument_documentHighlight);
	register_handler("textDocument/documentSymbol", &Protocol::on_textDocument_documentSymbol);
	register_handler("textDocument/formatting", &Protocol::on_textDocument_formatting);
	register_handler("textDocument/hover", &Protocol::on_textDocument_hover);
	register_handler("textDocument/onTypeFormatting", &Protocol::on_textDocument_onTypeFormatting);
	register_handler("textDocument/rangeFormatting", &Protocol::on_textDocument_rangeFormatting);
	register_handler("textDocument/rename", &Protocol::on_textDocument_rename);
	register_handler("textDocument/signatureHelp", &Protocol::on_textDocument_signatureHelp);
	register_handler("textDocument/switchSourceHeader", &Protocol::on_textDocument_switchSourceHeader);
	register_handler("workspace/didChangeConfiguration", &Protocol::on_workspace_didChangeConfiguration);
	register_handler("workspace/didChangeWatchedFiles", &Protocol::on_workspace_didChangeWatchedFiles);
	register_handler("workspace/executeCommand", &Protocol::on_workspace_executeCommand);
}

void Dispatcher::register_handler(const std::string &method, handler_type handler)
{
	_handlers[method] = std::move(handler);
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__ << " registered method " << method;
}

bool Dispatcher::call(rapidjson::Document &msg, std::ostream &output_stream) const
{
	if (!msg.HasMember("jsonrpc") || !msg["jsonrpc"].IsString() || msg["jsonrpc"] != "2.0")
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER did not find a valid jsonrpc message.";
		return false;
	}
	boost::optional<int> id;
	if (msg.HasMember("id"))
	{
		if (msg["id"].IsString())
		{
			id = std::stoi(msg["id"].GetString());
		}
		else
		{
			id = msg["id"].GetInt();
		}
	}
	if (!msg.HasMember("method") || !msg["method"].IsString())
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER did not find a method member in the json message.";
		return false;
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER create context_with_request_output_stream";
	Scoped_context_with_value context_with_request_output_stream(request_output_stream, &output_stream);
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER create context_with_id";
	boost::optional<Scoped_context_with_value> context_with_id;
	if (id)
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER emplace ID in context_with_id";
		context_with_id.emplace(request_id, *id);
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Searching for a handler for method " << msg["method"].GetString();
	auto handler = _handlers.find(msg["method"].GetString());
	if (handler != _handlers.end())
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Found a handler and calling it";
		if (msg.HasMember("params") && !msg["params"].IsNull())
		{
			handler->second(std::move(msg["params"].GetObject()));
		}
		else
		{
			handler->second(rapidjson::Value(rapidjson::kObjectType));
		}
	}
	else
	{
		_error_handler(rapidjson::Value(rapidjson::kObjectType));
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER exiting the scope of context_with_id and context_with_request_output_stream";
	return true;
}

void reply(rapidjson::Value &result)
{
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER get request_id from context";
	auto id = Context::get_current().get_value(request_id);
	if (!id)
	{
		BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__ << " no ID, no reply";
		return;
	}
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value root(rapidjson::kObjectType);
	root.AddMember("jsonrpc", rapidjson::Value("2.0").Move(), allocator);
	root.AddMember("id", rapidjson::Value(*id).Move(), allocator);
	root.AddMember("result", result, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string content(buffer.GetString());
	*(Context::get_current().get_existing(request_output_stream)) << "Content-Length: " << content.size() << "\r\n\r\n" << content;
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Sent response " << "Content-Length: " << content.size() << "\r\n\r\n" << content;
}

void reply(ERROR_CODES code, const std::string &msg)
{
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER get request_id from context";
	auto id = Context::get_current().get_value(request_id);
	if (!id)
	{
		BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__ << " no ID, no reply";
		return;
	}
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kObjectType);
	result.AddMember("code", rapidjson::Value(static_cast<int>(code)).Move(), allocator);
	result.AddMember("message", rapidjson::Value(rapidjson::StringRef(msg.c_str())).Move(), allocator);
	rapidjson::Value root(rapidjson::kObjectType);
	root.AddMember("jsonrpc", rapidjson::Value("2.0").Move(), allocator);
	root.AddMember("id", rapidjson::Value(*id).Move(), allocator);
	root.AddMember("error", result, allocator);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string content(buffer.GetString());
	*(Context::get_current().get_existing(request_output_stream)) << "Content-Length: " << content.size() << "\r\n\r\n" << content;
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Sent response " << "Content-Length: " << content.size() << "\r\n\r\n" << content;
}
