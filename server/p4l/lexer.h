/*
 * -*- c++ -*-
 */

#pragma once

#include <istream>
#include <string>

namespace p4l {

enum class Token {
  END              =  -1,
  // KEYWORDS
  PRAGMA           =  -2,
  END_PRAGMA       =  -3,
  ABSTRACT         =  -4,
  ACTION           =  -5,
  ACTIONS          =  -6,
  APPLY            =  -7,
  BOOL             =  -8,
  BIT              =  -9,
  CONST            = -10,
  CONTROL          = -11,
  DEFAULT          = -12,
  ELSE             = -13,
  ENTRIES          = -14,
  ENUM             = -15,
  ERROR            = -16,
  EXIT             = -17,
  EXTERN           = -18,
  FALSE            = -19,
  HEADER           = -20,
  HEADER_UNION     = -21,
  IF               = -22,
  IN               = -23,
  INOUT            = -24,
  INT              = -25,
  KEY              = -26,
  MATCH_KIND       = -27,
  TYPE             = -28,
  OUT              = -29,
  PARSER           = -30,
  PACKAGE          = -31,
  RETURN           = -32,
  SELECT           = -33,
  STATE            = -34,
  STRUCT           = -35,
  SWITCH           = -36,
  TABLE            = -37,
  THIS             = -38,
  TRANSITION       = -39,
  TRUE             = -40,
  TUPLE            = -41,
  TYPEDEF          = -42,
  VARBIT           = -43,
  VALUE_SET        = -44,
  VOID             = -45,
  DONTCARE         = -46,
  // PRIMARY
  IDENTIFIER       = -47,
  TYPE_IDENTIFIER  = -48,
  NUMBER           = -49,
  // OPERATORS
  MASK             = -50,
  RANGE            = -51,
  SHL              = -52,
  AND              = -53,
  OR               = -54,
  EQ               = -55,
  NE               = -56,
  GE               = -57,
  LE               = -58,
  PP               = -59,
  PLUS             = -60,
  PLUS_SAT         = -61,
  MINUS            = -62,
  MINUS_SAT        = -63,
  MUL              = -64,
  DIV              = -65,
  MOD              = -66,
  BIT_OR           = -67,
  BIT_AND          = -68,
  BIT_XOR          = -69,
  COMPLEMENT       = -70,
  L_PAREN          = -71,
  R_PAREN          = -72,
  L_BRACKET        = -73,
  R_BRACKET        = -74,
  L_BRACE          = -75,
  R_BRACE          = -76,
  L_ANGLE          = -77,
  R_ANGLE          = -78,
  NOT              = -79,
  COLON            = -80,
  COMMA            = -81,
  QUESTION         = -82,
  DOT              = -83,
  ASSIGN           = -84,
  SEMICOLON        = -85,
  AT               = -86,
  STRING_LITERAL   = -87,
  START            = -88,
  UNEXPECTED_TOKEN = -89
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

} // namespace p4l

std::ostream& operator<<(std::ostream& os, const p4l::Token& tokens);
