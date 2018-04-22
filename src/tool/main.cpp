#define BOOST_LOG_USE_NATIVE_SYSLOG

#include "lsp_server.h"

#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
// for std::cin, std::cout, std::clog
#include <iostream>


void init_logging_sink()
{
	boost::log::add_common_attributes();
	if (false)
	{
		// Create a syslog sink
		using ss = boost::log::sinks::synchronous_sink<boost::log::sinks::syslog_backend>;
		boost::shared_ptr<ss> sink(new ss(boost::log::keywords::use_impl = boost::log::sinks::syslog::native, boost::log::keywords::facility = boost::log::sinks::syslog::local7));
		sink->set_formatter(boost::log::expressions::format("%1%: %2%") % boost::log::expressions::attr<unsigned int>("LineID") % boost::log::expressions::smessage);
		sink->locked_backend()->set_severity_mapper(boost::log::sinks::syslog::direct_severity_mapping<int>("Severity"));
		boost::log::core::get()->add_sink(sink);
	}
	else
	{
		using ts = boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>;
		boost::shared_ptr<ts> sink(new ts());
		boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
		sink->set_formatter(boost::log::expressions::stream
							<< boost::log::expressions::attr<unsigned int>("LineID")
							<< ":" << boost::log::expressions::attr<boost::log::attributes::current_process_id::value_type>("ProcessID")
							<< ":" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
							<< " " << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
							<< " [" << boost::log::expressions::attr<int>("Severity") << "] "
							<< boost::log::expressions::smessage);
		sink->locked_backend()->add_stream(stream);
		boost::log::core::get()->add_sink(sink);
	}
}

int main(int argc, char* argv[])
{
	init_logging_sink();
	boost::log::sources::severity_logger<int> lg(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
	BOOST_LOG_SEV(lg, boost::log::sinks::syslog::debug) << "STARTED";
	LSP_server the_server(std::cin, std::cout);
	return the_server.run();
}
