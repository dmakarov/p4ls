#include "context.h"

boost::log::sources::severity_logger<int> Context::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

static Context& get_instance() {
	thread_local static auto context = Context::create_empty();
	return context;
}

Context Context::create_empty()
{
	return Context(nullptr);
}

const Context& Context::get_current()
{
	return get_instance();
}

Context Context::swap_current(Context other)
{
	std::swap(other, get_instance());
	return other;
}

std::ostream& operator<<(std::ostream& os, const Context::Data& data)
{
	os << "key \"" << data._key << "\" value \"";
	if (data._value)
	{
		os << data._value->to_string();
	}
	else
	{
		os << "(null)";
	}
	os << "\" parent {";
	if (data._parent)
	{
		os << *data._parent;
	}
	else
	{
		os << "(null)";
	}
	os << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Context& context)
{
	if (context._data)
	{
		os << "{" << *context._data << "}";
	}
	else
	{
		os << "{(null)}";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Scoped_context& context)
{
	os << context._previous;
	return os;
}
