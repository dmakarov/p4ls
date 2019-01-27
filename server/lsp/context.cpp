#include "context.h"

#include <boost/log/attributes/constant.hpp>

boost::log::sources::severity_logger<int> Context::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

static Context& get_instance() {
	thread_local static auto context = Context::create_empty();
	return context;
}

Context Context::create_empty()
{
	Context::_logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("CONTEXT"));
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
	os << "key:\"" << data._key
	   << "\", value:\"" << (data._value ? data._value->to_string() : "(null)")
	   << "\", parent:{";
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
