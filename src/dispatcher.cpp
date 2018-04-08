#include "dispatcher.h"

void Dispatcher::register_handler(const std::string &method, handler_type handler)
{
	handlers[method] = std::move(handler);
}

bool Dispatcher::call(rapidjson::Document &msg) const
{
	return true;
}
