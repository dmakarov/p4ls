#include "context.h"

boost::log::sources::severity_logger<int> Context::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

static Context &create_empty_context() {
	BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	thread_local static auto context = Context::get_empty();
	return context;
}

Context Context::get_empty()
{
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	return Context(nullptr);
}

const Context &Context::get_current()
{
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	return create_empty_context();
}

Context Context::swap_current(Context other)
{
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
	std::swap(other, create_empty_context());
	return other;
}
