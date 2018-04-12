#include "dispatcher.h"
#include "context.h"

#include <boost/log/trivial.hpp>
#include <boost/optional.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace {

static Key<int> request_id;
static Key<std::ostream *> request_output_stream;

} // namespace


void Dispatcher::register_handler(const std::string &method, handler_type handler)
{
	_handlers[method] = std::move(handler);
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__ << " registered method " << method;
}

bool Dispatcher::call(rapidjson::Document &msg, std::ostream &output_stream) const
{
	if (!msg.HasMember("jsonrpc") || !msg["jsonrpc"].IsString() || msg["jsonrpc"] != "2.0")
	{
		return false;
	}
	boost::optional<int> id;
	if (msg.HasMember("id"))
	{
		id = msg["id"].GetInt();
	}
	if (!msg.HasMember("method") || !msg["method"].IsString())
	{
		return false;
	}
	auto handler = _handlers.find(msg["method"].GetString());
	Scoped_context_with_value context_with_request_output_stream(request_output_stream, &output_stream);
	boost::optional<Scoped_context_with_value> context_with_id;
	if (id)
	{
		context_with_id.emplace(request_id, *id);
	}
	if (handler != _handlers.end())
	{
		handler->second(std::move(msg["params"].GetObject()));
	}
	return true;
}

void reply(rapidjson::Value &result)
{
	auto id = Context::get_current().get_value(request_id);
	if (!id)
	{
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
	*(Context::get_current().get_existing(request_output_stream)) << buffer.GetString();
	BOOST_LOG_TRIVIAL(info) << "Sent response " << buffer.GetString();
}
