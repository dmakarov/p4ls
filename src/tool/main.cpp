#include "lsp_server.h"

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
// for std::cin
#include <iostream>


int main(int argc, char* argv[])
{
	boost::log::add_file_log("p4lsd.log");
	BOOST_LOG_TRIVIAL(info) << "p4lsd started";
	LSP_server the_server;
	return the_server.run(std::cin);
}
