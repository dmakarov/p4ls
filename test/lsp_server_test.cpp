#include "lsp_server.h"

#include <boost/test/unit_test.hpp>

#include <sstream>


BOOST_AUTO_TEST_SUITE(lsp_server_test_suite);

BOOST_AUTO_TEST_CASE(test_case1)
{
    std::istringstream input{"{\"project\":\"rapidjson\",\"stars\":10}"};
    LSP_server lsp;
    BOOST_REQUIRE_EQUAL(lsp.run(input), 0);
}

BOOST_AUTO_TEST_SUITE_END();
