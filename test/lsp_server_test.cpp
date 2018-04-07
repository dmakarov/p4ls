#include "lsp_server.h"

#include <boost/test/unit_test.hpp>

#include <sstream>


BOOST_AUTO_TEST_SUITE(lsp_server_test_suite);

BOOST_AUTO_TEST_CASE(test_case1)
{
	std::istringstream input{"Content-Length: 125\r\nContent-Type: application/emacs-jsonrpc; charset=utf-8\r\n\r\n{\"jsonrpc\":\"2.0\",\"id\":0,\"method\":\"initialize\",\"params\":{\"processId\":123,\"rootPath\":\"clangd\",\"capabilities\":{},\"trace\":\"off\"}}"};
	LSP_server lsp;
	BOOST_REQUIRE_EQUAL(lsp.run(input), 0);
}

BOOST_AUTO_TEST_SUITE_END();
