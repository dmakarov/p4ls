#include <boost/test/unit_test.hpp>
#include <boost/wave.hpp>

#include <sstream>

#include "lexer.h"

BOOST_AUTO_TEST_SUITE(wave_test_suite);

BOOST_AUTO_TEST_CASE(test_main)
{
	std::string input_file_name("main.p4");
	std::ifstream ifs(input_file_name);
	BOOST_TEST(ifs.is_open());

	ifs.unsetf(std::ios::skipws);
	auto teststr = std::string(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());

	using token_type = p4l::p4lex_token<>;
	using lexer_type = p4l::p4lex_iterator<token_type>;
	using context_type = boost::wave::context<std::string::iterator, lexer_type>;

	context_type ctx(teststr.begin(), teststr.end(), input_file_name.c_str());

	ctx.set_language(boost::wave::support_cpp0x);
	ctx.set_language(boost::wave::enable_preserve_comments(ctx.get_language()));
	ctx.set_language(boost::wave::enable_prefer_pp_numbers(ctx.get_language()));
	ctx.set_language(boost::wave::enable_emit_contnewlines(ctx.get_language()));

	context_type::token_type current_token;

	try {
		for (auto& token : ctx) {
			std::cout << "matched " << token << std::endl;
		}
	} catch (boost::wave::cpp_exception const& e) {
		std::cerr << e.file_name() << "(" << e.line_no() << "): " << e.description() << std::endl;
	} catch (std::exception const& e) {
		std::cerr << current_token.get_position().get_file() << "(" << current_token.get_position().get_line() << "): " << "unexpected exception: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << current_token.get_position().get_file() << "(" << current_token.get_position().get_line() << "): " << "unexpected exception." << std::endl;
	}
}

BOOST_AUTO_TEST_SUITE_END();
