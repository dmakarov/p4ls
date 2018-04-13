#include "lsp_server.h"

#include <boost/test/unit_test.hpp>

#include <sstream>


BOOST_AUTO_TEST_SUITE(lsp_server_test_suite);

BOOST_AUTO_TEST_CASE(test_case1)
{
	std::istringstream input{"Content-Length: 465\r\n"
							 "Content-Type: application/emacs-jsonrpc; charset=utf-8\r\n"
							 "\r\n"
							 "{\"jsonrpc\":\"2.0\","
							 "\"method\":\"initialize\","
							 "\"params\":{\"processId\":20943,"
							 "\"rootPath\":\"/Users/dmakarov/work/try/p4ls/\","
							 "\"rootUri\":\"file:///Users/dmakarov/work/try/p4ls/\","
							 "\"capabilities\":{"
							 "\"workspace\":{"
							 "\"applyEdit\":true,"
							 "\"executeCommand\":{"
							 "\"dynamicRegistration\":true}},"
							 "\"textDocument\":{"
							 "\"synchronization\":{"
							 "\"willSave\":true,"
							 "\"didSave\":true},"
							 "\"symbol\":{"
							 "\"symbolKind\":{"
							 "\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]}}}},"
							 "\"initializationOptions\":null},"
							 "\"id\":1}"};
	std::ostringstream output;
	LSP_server lsp(input, output);
	BOOST_REQUIRE_EQUAL(lsp.run(), 0);
}

BOOST_AUTO_TEST_CASE(test_case2)
{
	std::istringstream input{"Content-Length: 125\r\n"
							 "Content-Type: application/emacs-jsonrpc; charset=utf-8\r\n"
							 "\r\n"
							 "{\"jsonrpc\":\"2.0\",\"id\":0,\"method\":\"initialize\",\"params\":{\"processId\":123,\"rootPath\":\"clangd\",\"capabilities\":{},\"trace\":\"off\"}}"
							 "Content-Length: 44\r\n"
							 "Content-Type: application/emacs-jsonrpc; charset=utf-8\r\n"
							 "\r\n"
							 "{\"jsonrpc\":\"2.0\",\"id\":0,\"method\":\"shutdown\"}"
	};
	std::ostringstream output;
	LSP_server lsp(input, output);
	BOOST_REQUIRE_EQUAL(lsp.run(), 0);
}

BOOST_AUTO_TEST_SUITE_END();
