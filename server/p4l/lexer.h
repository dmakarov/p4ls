/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/wave.hpp>

#include <istream>
#include <string>

namespace p4l {

enum class Token {
  P4L_END              =  -1,
  // KEYWORDS
  P4L_PRAGMA           =  -2,
  P4L_END_PRAGMA       =  -3,
  P4L_ABSTRACT         =  -4,
  P4L_ACTION           =  -5,
  P4L_ACTIONS          =  -6,
  P4L_APPLY            =  -7,
  P4L_BOOL             =  -8,
  P4L_BIT              =  -9,
  P4L_CONST            = -10,
  P4L_CONTROL          = -11,
  P4L_DEFAULT          = -12,
  P4L_ELSE             = -13,
  P4L_ENTRIES          = -14,
  P4L_ENUM             = -15,
  P4L_ERROR            = -16,
  P4L_EXIT             = -17,
  P4L_EXTERN           = -18,
  P4L_FALSE            = -19,
  P4L_HEADER           = -20,
  P4L_HEADER_UNION     = -21,
  P4L_IF               = -22,
  P4L_IN               = -23,
  P4L_INOUT            = -24,
  P4L_INT              = -25,
  P4L_KEY              = -26,
  P4L_MATCH_KIND       = -27,
  P4L_TYPE             = -28,
  P4L_OUT              = -29,
  P4L_PARSER           = -30,
  P4L_PACKAGE          = -31,
  P4L_RETURN           = -32,
  P4L_SELECT           = -33,
  P4L_STATE            = -34,
  P4L_STRUCT           = -35,
  P4L_SWITCH           = -36,
  P4L_TABLE            = -37,
  P4L_THIS             = -38,
  P4L_TRANSITION       = -39,
  P4L_TRUE             = -40,
  P4L_TUPLE            = -41,
  P4L_TYPEDEF          = -42,
  P4L_VARBIT           = -43,
  P4L_VALUE_SET        = -44,
  P4L_VOID             = -45,
  P4L_DONTCARE         = -46,
  // PRIMARY
  P4L_IDENTIFIER       = -47,
  P4L_TYPE_IDENTIFIER  = -48,
  P4L_NUMBER           = -49,
  // OPERATORS
  P4L_MASK             = -50,
  P4L_RANGE            = -51,
  P4L_SHL              = -52,
  P4L_AND              = -53,
  P4L_OR               = -54,
  P4L_EQ               = -55,
  P4L_NE               = -56,
  P4L_GE               = -57,
  P4L_LE               = -58,
  P4L_PP               = -59,
  P4L_PLUS             = -60,
  P4L_PLUS_SAT         = -61,
  P4L_MINUS            = -62,
  P4L_MINUS_SAT        = -63,
  P4L_MUL              = -64,
  P4L_DIV              = -65,
  P4L_MOD              = -66,
  P4L_BIT_OR           = -67,
  P4L_BIT_AND          = -68,
  P4L_BIT_XOR          = -69,
  P4L_COMPLEMENT       = -70,
  P4L_L_PAREN          = -71,
  P4L_R_PAREN          = -72,
  P4L_L_BRACKET        = -73,
  P4L_R_BRACKET        = -74,
  P4L_L_BRACE          = -75,
  P4L_R_BRACE          = -76,
  P4L_L_ANGLE          = -77,
  P4L_R_ANGLE          = -78,
  P4L_NOT              = -79,
  P4L_COLON            = -80,
  P4L_COMMA            = -81,
  P4L_QUESTION         = -82,
  P4L_DOT              = -83,
  P4L_ASSIGN           = -84,
  P4L_SEMICOLON        = -85,
  P4L_AT               = -86,
  P4L_STRING_LITERAL   = -87,
  P4L_START            = -88,
  P4L_UNEXPECTED_TOKEN = -89
};

class Lexer final {
 public:
	explicit Lexer(std::istream& is) :is(is) {}
	~Lexer() = default;
	Token next();
	std::string get_text() {
		return text;
	}
	int get_value() {
		return value;
	}
	int get_width() {
		return width;
	}
	int get_row() {
		return row;
	}
	int get_col() {
		return col;
	}

 private:
	Token read_other();
	Token read_number();
	Token read_pragma();
	Token read_string();
	Token read_identifier();
	void read_comment();
	void read_location();
	char advance() {
		if (last == EOF) {
			return last;
		}
		last = is.get();
		if (last == '\n') {
			++row;
			col = 0;
		} else {
			++col;
		}
		return last;
	}

	std::string text;
	int value = 0;
	int width = 32;

	std::istream& is;
	char last = ' ';
	int row = 1;
	int col = 0;
};

template <typename PositionT>
class lex_token {
 public:
	using position_type = PositionT;
};

struct lex_iterator {
	using token_type = lex_token<boost::wave::util::file_position_type>;
	using iterator_category = std::forward_iterator_tag;

	lex_iterator();

	template <typename Iterator>
	lex_iterator(Iterator const &first, Iterator const& last,
				 typename token_type::position_type const& pos,
				 boost::wave::language_support language);
};

} // namespace p4l

std::ostream& operator<<(std::ostream& os, const p4l::Token& tokens);
