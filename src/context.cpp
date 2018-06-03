#include "context.h"

boost::log::sources::severity_logger<int> Context::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

static Context &create_empty_context() {
#if LOGGING_ENABLED
	BOOST_LOG_SEV(Context::_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
#endif
	thread_local static auto context = Context::get_empty();
	return context;
}

Context Context::get_empty()
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
#endif
	return Context(nullptr);
}

const Context &Context::get_current()
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
#endif
	return create_empty_context();
}

Context Context::swap_current(Context other)
{
#if LOGGING_ENABLED
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::debug) << __PRETTY_FUNCTION__;
#endif
	std::swap(other, create_empty_context());
	return other;
}
