#include "lexer.h"

#include <boost/test/unit_test.hpp>

#include <sstream>

namespace p4l {
std::ostream& boost_test_print_type(std::ostream& os, const Token& tok) {
	os << tok;
	return os;
}
}

BOOST_AUTO_TEST_SUITE(lexer_test_suite);

BOOST_AUTO_TEST_CASE(test_numeric_literals)
{
	std::istringstream input{
		"12345 2s3 8w10 8s10 1w10 1s10"
	};
	p4l::Lexer lexer(input);
	auto token = lexer.next();
	auto value = lexer.get_value();
	auto width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 12345);
	BOOST_REQUIRE_EQUAL(width, 32);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, -1);
	BOOST_REQUIRE_EQUAL(width, 2);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 10);
	BOOST_REQUIRE_EQUAL(width, 8);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 10);
	BOOST_REQUIRE_EQUAL(width, 8);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 0);
	BOOST_REQUIRE_EQUAL(width, 1);
	token = lexer.next();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_UNEXPECTED_TOKEN);
}

BOOST_AUTO_TEST_CASE(test_operators)
{
	std::istringstream input{
		"abc&0x19&&&8w0x0f"
	};
	p4l::Lexer lexer(input);
	auto token = lexer.next();
	auto value = lexer.get_value();
	auto width = lexer.get_width();
	auto text = lexer.get_text();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_IDENTIFIER);
	BOOST_REQUIRE_EQUAL(value, 0);
	BOOST_REQUIRE_EQUAL(width, 32);
	BOOST_REQUIRE_EQUAL(text, "abc");
	token = lexer.next();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_BIT_AND);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 25);
	BOOST_REQUIRE_EQUAL(width, 32);
	token = lexer.next();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_MASK);
	token = lexer.next();
	value = lexer.get_value();
	width = lexer.get_width();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_NUMBER);
	BOOST_REQUIRE_EQUAL(value, 15);
	BOOST_REQUIRE_EQUAL(width, 8);
	token = lexer.next();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_END);
}

BOOST_AUTO_TEST_CASE(test_comments)
{
	std::istringstream input{
	//   1                2             3  4            5
	//            1                 1                1            1         2         3
	//   12345678901234567 1234567890123 12 123456789012 1234567890123456789012345678901
		"abc // some text\n  def /*****\n*\n* more text\n ******/another_identifier /**/"
	};
	p4l::Lexer lexer(input);
	auto token = lexer.next();
	auto text = lexer.get_text();
	auto row = lexer.get_row();
	auto col = lexer.get_col();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_IDENTIFIER);
	BOOST_REQUIRE_EQUAL(text, "abc");
	BOOST_REQUIRE_EQUAL(row, 1);
	BOOST_REQUIRE_EQUAL(col, 4);
	token = lexer.next();
	text = lexer.get_text();
	row = lexer.get_row();
	col = lexer.get_col();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_IDENTIFIER);
	BOOST_REQUIRE_EQUAL(text, "def");
	BOOST_REQUIRE_EQUAL(row, 2);
	BOOST_REQUIRE_EQUAL(col, 6);
	token = lexer.next();
	text = lexer.get_text();
	row = lexer.get_row();
	col = lexer.get_col();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_IDENTIFIER);
	BOOST_REQUIRE_EQUAL(text, "another_identifier");
	BOOST_REQUIRE_EQUAL(row, 5);
	BOOST_REQUIRE_EQUAL(col, 27);
	token = lexer.next();
	row = lexer.get_row();
	col = lexer.get_col();
	BOOST_REQUIRE_EQUAL(token, p4l::Token::P4L_END);
	BOOST_REQUIRE_EQUAL(row, 5);
	BOOST_REQUIRE_EQUAL(col, 32);
}

BOOST_AUTO_TEST_SUITE_END();
