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
#include <boost/optional.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <csignal>
// for std::cin, std::cout, std::clog
#include <fstream>
#include <iostream>
#include <string>

namespace
{
	boost::optional<boost::shared_ptr<std::ofstream>> log_file_stream;
}

void signal_handler(int signo)
{
	if (log_file_stream)
	{
		log_file_stream.get()->close();
	}
	std::exit(0);
}

void init_logging_sink(const boost::optional<boost::shared_ptr<std::ofstream>>& file_stream)
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
		sink->set_formatter(boost::log::expressions::stream
							<< std::setw(6) << boost::log::expressions::attr<unsigned int>("LineID")
							<< ":" << boost::log::expressions::attr<boost::log::attributes::current_process_id::value_type>("ProcessID")
							<< ":" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
							<< " " << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
							<< " [" << boost::log::expressions::attr<int>("Severity") << "] "
							<< boost::log::expressions::smessage);
		boost::shared_ptr<std::ostream> clog_stream(&std::clog, boost::null_deleter());
		sink->locked_backend()->add_stream(clog_stream);
		if (file_stream)
		{
			sink->locked_backend()->add_stream(file_stream.get());
		}
		boost::log::core::get()->add_sink(sink);
	}
}


int main(int argc, char* argv[])
{
	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	if (argc > 1)
	{
		if (std::string("-l") == argv[1])
		{
			if (argc > 2)
			{
				log_file_stream.emplace(boost::make_shared<std::ofstream>(argv[2], std::ios::app));
			}
		}
	}
	init_logging_sink(log_file_stream);
	boost::log::sources::severity_logger<int> lg(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
	BOOST_LOG_SEV(lg, boost::log::sinks::syslog::debug) << "STARTED";
	LSP_server the_server(std::cin, std::cout);
	auto status = the_server.run();
	if (log_file_stream)
	{
		log_file_stream.get()->close();
	}
	return status;
}
