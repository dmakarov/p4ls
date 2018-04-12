#define BOOST_LOG_USE_NATIVE_SYSLOG

#include "lsp_server.h"

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
// for std::cin
#include <iostream>


void init_native_syslog()
{
	// Create a syslog sink
	using ss = boost::log::sinks::synchronous_sink<boost::log::sinks::syslog_backend>;
	boost::shared_ptr<ss> sink(new ss(boost::log::keywords::use_impl = boost::log::sinks::syslog::native, boost::log::keywords::facility = boost::log::sinks::syslog::local7));
	sink->set_formatter(boost::log::expressions::format("%1%: %2%") % boost::log::expressions::attr<unsigned int>("RecordID") % boost::log::expressions::smessage);
	sink->locked_backend()->set_severity_mapper(boost::log::sinks::syslog::direct_severity_mapping<int>("Severity"));
	// Add the sink to the core
	boost::log::core::get()->add_sink(sink);
	// Add some attributes too
	boost::log::core::get()->add_global_attribute("RecordID", boost::log::attributes::counter<unsigned int>());
}

int main(int argc, char* argv[])
{
	init_native_syslog();
	boost::log::sources::severity_logger<int> lg(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
	BOOST_LOG_SEV(lg, boost::log::sinks::syslog::notice) << "STARTED";
	LSP_server the_server(std::cin, std::cout);
	return the_server.run();
}
