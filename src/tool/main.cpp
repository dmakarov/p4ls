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

#include <execinfo.h>
#include <unistd.h>


namespace {
boost::optional<boost::shared_ptr<std::ofstream>> log_file_stream;
boost::log::sources::severity_logger<int> logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);
const char* signals[] = { "", "SIGHUP", "SIGINT", "SIGQUIT",
						  "SIGILL", "SIGTRAP", "SIGABRT", "SIGEMT",
						  "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV",
						  "SIGSYS", "SIGPIPE", "SIGALRM", "SIGTERM",
						  "SIGURG", "SIGSTOP", "SIGTSTP", "SIGCONT",
						  "SIGCHLD", "SIGTTIN", "SIGTTOU", "SIGIO"
						  "SIGZCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF"
						  "SIGWINCH", "SIGINFO", "SIGUSR1", "SIGUSR2"};
} // namespace

void signal_handler(int signo)
{
	if (log_file_stream)
	{
		BOOST_LOG_SEV(logger, boost::log::sinks::syslog::debug) << "TERMINATED " << signals[signo] << "(" << signo << ")";
		log_file_stream.get()->close();
	}
	if (signo == SIGSEGV || signo == SIGBUS)
	{
		static void *buffer[64];
		auto size = backtrace(buffer, 64);
		char **strings = backtrace_symbols(buffer, size);
		if (strings)
		{
			for (int i = 1; i < size; ++i)
			{
				BOOST_LOG_SEV(logger, boost::log::sinks::syslog::debug) << strings[i];
			}
			free(strings);
		}
		if (size < 1)
		{
			BOOST_LOG_SEV(logger, boost::log::sinks::syslog::debug) << "backtrace failed";
		}
	}
	std::exit(0);
}

void init_logging_sink(const boost::optional<boost::shared_ptr<std::ofstream>>& file_stream)
{
	boost::log::add_common_attributes();
	using ts = boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>;
	boost::shared_ptr<ts> sink(new ts());
	sink->set_formatter(boost::log::expressions::stream
						<< std::setw(6) << boost::log::expressions::attr<unsigned int>("LineID")
						<< ":" << boost::log::expressions::attr<boost::log::attributes::current_process_id::value_type>("ProcessID")
						<< ":" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
						<< " " << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
						<< " [" << boost::log::expressions::attr<int>("Severity") << "] "
						<< boost::log::expressions::attr<std::string>("Tag") << ": "
						<< boost::log::expressions::smessage);
	if (file_stream)
	{
		sink->locked_backend()->add_stream(file_stream.get());
	}
	else
	{
		boost::shared_ptr<std::ostream> clog_stream(&std::clog, boost::null_deleter());
		sink->locked_backend()->add_stream(clog_stream);
	}
	sink->locked_backend()->auto_flush(true);
	boost::log::core::get()->add_sink(sink);
	logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("P$LSD"));
}


int main(int argc, char* argv[])
{
	std::signal(SIGABRT, signal_handler);
	std::signal(SIGBUS, signal_handler);
	std::signal(SIGHUP, signal_handler);
	std::signal(SIGINT, signal_handler);
	std::signal(SIGKILL, signal_handler);
	std::signal(SIGSEGV, signal_handler);
	std::signal(SIGTERM, signal_handler);
	boost::optional<std::istream&> input(std::cin);
	std::ifstream ifs;
	for (auto index = 1; index < argc; ++index)
	{
		if (std::string("-l") == argv[index])
		{
			if (++index < argc)
			{
				log_file_stream.emplace(boost::make_shared<std::ofstream>(argv[index], std::ios::app));
			}
		}
		else if (std::string("-i") == argv[index])
		{
			if (++index < argc)
			{
				ifs.open(argv[index]);
				input.emplace(ifs);
			}
		}
	}
	init_logging_sink(log_file_stream);
	BOOST_LOG_SEV(logger, boost::log::sinks::syslog::debug) << "STARTED";
	LSP_server the_server(*input, std::cout);
	auto status = the_server.run();
	if (log_file_stream)
	{
		log_file_stream.get()->close();
	}
	return status;
}
