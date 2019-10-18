#include <boost/test/unit_test.hpp>
#include <boost/wave.hpp>

#include <sstream>

#include "lexer.h"
#include "main.p4"

BOOST_AUTO_TEST_SUITE(wave_test_suite);

BOOST_AUTO_TEST_CASE(test_main)
{
	using token_type = p4l::p4lex_token<>;
	using lexer_type = p4l::p4lex_iterator<token_type>;
	using context_type = boost::wave::context<std::string::iterator, lexer_type>;
	std::string teststr(source_code);
	context_type ctx(teststr.begin(), teststr.end(), "main.p4");
	ctx.set_language(boost::wave::support_cpp0x);
	ctx.set_language(boost::wave::enable_preserve_comments(ctx.get_language()));
	ctx.set_language(boost::wave::enable_prefer_pp_numbers(ctx.get_language()));
	ctx.set_language(boost::wave::enable_emit_contnewlines(ctx.get_language()));
	context_type::token_type current_token;
	auto token = ctx.begin();
	while (token != ctx.end()) {
		try {
			std::cout << "matched " << *token << std::endl;
			++token;
		} catch (boost::wave::cpp_exception const& e) {
			std::cerr << e.file_name() << "(" << e.line_no() << "): " << e.description() << std::endl;
		} catch (std::exception const& e) {
			std::cerr << current_token.get_position().get_file() << "(" << current_token.get_position().get_line() << "): " << "unexpected exception: " << e.what() << std::endl;
		} catch (...) {
			std::cerr << current_token.get_position().get_file() << "(" << current_token.get_position().get_line() << "): " << "unexpected exception." << std::endl;
		}
	}
}

BOOST_AUTO_TEST_SUITE_END();
