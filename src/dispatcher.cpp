#include "dispatcher.h"
#include "context.h"

#include <boost/optional.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

boost::log::sources::severity_logger<int> Dispatcher::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
const std::string Dispatcher::_JSONRPC_VERSION("2.0");

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
	register_handler("textDocument/codeLens", &Protocol::on_textDocument_codeLens);
	register_handler("codeLens/resolve", &Protocol::on_codeLens_resolve);
	register_handler("textDocument/completion", &Protocol::on_textDocument_completion);
	register_handler("textDocument/definition", &Protocol::on_textDocument_definition);
	register_handler("textDocument/didChange", &Protocol::on_textDocument_didChange);
	register_handler("textDocument/didClose", &Protocol::on_textDocument_didClose);
	register_handler("textDocument/didOpen", &Protocol::on_textDocument_didOpen);
	register_handler("textDocument/didSave", &Protocol::on_textDocument_didSave);
	register_handler("textDocument/documentHighlight", &Protocol::on_textDocument_documentHighlight);
	register_handler("textDocument/documentSymbol", &Protocol::on_textDocument_documentSymbol);
	register_handler("textDocument/formatting", &Protocol::on_textDocument_formatting);
	register_handler("textDocument/hover", &Protocol::on_textDocument_hover);
	register_handler("textDocument/implementation", &Protocol::on_textDocument_implementation);
	register_handler("textDocument/onTypeFormatting", &Protocol::on_textDocument_onTypeFormatting);
	register_handler("textDocument/rangeFormatting", &Protocol::on_textDocument_rangeFormatting);
	register_handler("textDocument/rename", &Protocol::on_textDocument_rename);
	register_handler("textDocument/signatureHelp", &Protocol::on_textDocument_signatureHelp);
	register_handler("textDocument/switchSourceHeader", &Protocol::on_textDocument_switchSourceHeader);
	register_handler("textDocument/typeDefinition", &Protocol::on_textDocument_typeDefinition);
	register_handler("workspace/didChangeConfiguration", &Protocol::on_workspace_didChangeConfiguration);
	register_handler("workspace/didChangeWatchedFiles", &Protocol::on_workspace_didChangeWatchedFiles);
	register_handler("workspace/executeCommand", &Protocol::on_workspace_executeCommand);
}

void Dispatcher::register_handler(const std::string &method, handler_type handler)
{
	_handlers[method] = std::move(handler);
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__ << " registered method " << method;
}

void Dispatcher::call(std::string content, std::ostream &output_stream) const
{
	rapidjson::Document msg;
	if (msg.Parse(content.c_str()).HasParseError())
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "DISPATCHER JSON parse error: " << rapidjson::GetParseError_En(msg.GetParseError()) << " (" << msg.GetErrorOffset() << ")";
		return;
	}
	auto it = msg.FindMember("jsonrpc");
	if (it == msg.MemberEnd() || !it->value.IsString() || it->value.GetString() != _JSONRPC_VERSION)
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "DISPATCHER did not find a valid jsonrpc message.";
		return;
	}
	boost::optional<int> id;
	it = msg.FindMember("id");
	if (it != msg.MemberEnd())
	{
		id = it->value.IsString() ? std::stoi(it->value.GetString()) : it->value.GetInt();
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER message is a request with id " << *id;
	}
	else
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER message is a note without id.";
	}
	it = msg.FindMember("method");
	if (it == msg.MemberEnd() || !it->value.IsString())
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error) << "DISPATCHER did not find a method member in the json message.";
		return;
	}
	Scoped_context_with_value context_with_request_output_stream(request_output_stream, &output_stream);
	boost::optional<Scoped_context_with_value> context_with_id;
	if (id)
	{
		context_with_id.emplace(request_id, *id);
	}
	std::string method(it->value.GetString());
	auto handler = _handlers.find(method);
	if (handler != _handlers.end())
	{
		it = msg.FindMember("params");
		if (it == msg.MemberEnd() || it->value.IsNull())
		{
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER invoking method \"" << method << "\" without parameters.";
			handler->second(std::move(rapidjson::Value(rapidjson::kObjectType)));
		}
		else
		{
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER invoking method \"" << method << "\" with parameters.";
			handler->second(std::move(msg["params"].GetObject()));
		}
	}
	else
	{
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER did not find method \"" << method << "\"";
		_error_handler(rapidjson::Value(rapidjson::kObjectType));
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << "DISPATCHER finished processing method \"" << method << "\"";
}

void reply(rapidjson::Value &result)
{
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER get request_id from context.";
	auto id = Context::get_current().get_value(request_id);
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER got id " << (id ? std::to_string(*id) : std::string("(null)"));
	if (!id)
	{
		BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER does not reply.";
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
	auto content_length = content.size() + 2;
	*(Context::get_current().get_existing(request_output_stream)) << "Content-Length: " << content_length << "\r\n\r\n" << content << "\r\n" << std::flush;
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Sent response " << "Content-Length: " << content_length << "\r\n\r\n" << content << "\r\n";
}

void reply(ERROR_CODES code, const std::string &msg)
{
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER get request_id from context.";
	auto id = Context::get_current().get_value(request_id);
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER got id " << (id ? std::to_string(*id) : std::string("(null)"));
	if (!id)
	{
		BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER does not reply.";
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
	auto content_length = content.size() + 2;
	*(Context::get_current().get_existing(request_output_stream)) << "Content-Length: " << content_length << "\r\n\r\n" << content << "\r\n" << std::flush;
	BOOST_LOG_SEV(Dispatcher::_logger, boost::log::sinks::syslog::debug) << "DISPATCHER Sent response " << "Content-Length: " << content_length << "\r\n\r\n" << content << "\r\n";
}
