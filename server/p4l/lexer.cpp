#include "lexer.h"

#include <regex>
#include <sstream>

namespace p4l {

int Lexer::next()
{
	while (isspace(last)) {
		advance();
	}
	if (last == EOF) {
		return boost::wave::T_END;
	}
	if (isalpha(last) || last == '_') {
		return read_identifier();
	}
	if (isdigit(last)) {
		return read_number();
	}
	return read_other();
}

int Lexer::read_other()
{
	auto prev = last;
	advance();
	switch (prev) {
	case '!': // "!=" "!"
		if (last == '=') {
			advance();
			return boost::wave::T_NE;
		}
		return boost::wave::T_NOT;
	case '"': return read_string();
	case '#':
		read_location();
		if (last == EOF) {
			return boost::wave::T_END;
		}
		return next();
	case '%': return boost::wave::T_MOD;
	case '&': // "&&&" "&&" "&"
		if (last == '&') {
			if (advance() == '&') {
				advance();
				return boost::wave::T_MASK;
			}
			return boost::wave::T_AND;
		}
		return boost::wave::T_BIT_AND;
	case '(': return boost::wave::T_L_PAREN;
	case ')': return boost::wave::T_R_PAREN;
	case '*': return boost::wave::T_MUL;
	case '+': // "++" "+"
		if (last == '+') {
			advance();
			return boost::wave::T_PP;
		}
		return boost::wave::T_PLUS;
	case ',': return boost::wave::T_COMMA;
	case '-': return boost::wave::T_MINUS;
	case '.': // ".." "."
		if (last == '.') {
			advance();
			return boost::wave::T_RANGE;
		}
		return boost::wave::T_DOT;
	case '/': // "//" "/*" "/"
		if (last == '/' || last == '*') {
			read_comment();
			return next();
		}
		return boost::wave::T_DIV;
	case ':': return boost::wave::T_COLON;
	case ';': return boost::wave::T_SEMICOLON;
	case '<': // "<<" "<=" "<"
		if (last == '<') {
			advance();
			return boost::wave::T_SHL;
		}
		if (last == '=') {
			advance();
			return boost::wave::T_LE;
		}
		return boost::wave::T_L_ANGLE;
	case '=': // "==" "="
		if (last == '=') {
			advance();
			return boost::wave::T_EQ;
		}
		return boost::wave::T_ASSIGN;
	case '>': // ">=" ">"
		if (last == '=') {
			advance();
			return boost::wave::T_GE;
		}
		return boost::wave::T_R_ANGLE;
	case '?': return boost::wave::T_QUESTION;
	case '@':
		if (last == 'p') {
			return read_pragma();
		}
		return boost::wave::T_AT;
	case '[': return boost::wave::T_L_BRACKET;
	case ']': return boost::wave::T_R_BRACKET;
	case '^': return boost::wave::T_BIT_XOR;
	case '{': return boost::wave::T_L_BRACE;
	case '|': // "|+|" "|-|" "||" "|"
		if (last == '+') {
			if (advance() == '|') {
				advance();
				return boost::wave::T_PLUS_SAT;
			}
			return boost::wave::T_UNKNOWN;
		}
		if (last == '-') {
			if (advance() == '|') {
				advance();
				return boost::wave::T_MINUS_SAT;
			}
			return boost::wave::T_UNKNOWN;
		}
		if (last == '|') {
			advance();
			return boost::wave::T_OR;
		}
		return boost::wave::T_BIT_OR;
	case '}': return boost::wave::T_R_BRACE;
	case '~': return boost::wave::T_COMPLEMENT;
	default:;
	}
	return boost::wave::T_UNKNOWN;
}

int Lexer::read_number()
{
	std::string numstr;
	do {
		numstr += last;
		advance();
	} while (isdigit(last)  || last == '_' || last == 'w' || last == 's'
			 || last == 'a' || last == 'A' || last == 'b' || last == 'B'
			 || last == 'c' || last == 'C' || last == 'd' || last == 'D'
			 || last == 'e' || last == 'E' || last == 'f' || last == 'F'
			 || last == 'o' || last == 'O' || last == 'x' || last == 'X');
	std::regex rx("(([0-9]+)([sw]))?(0[bBdDoOxX])?([0-9A-Fa-f_]+)");
	std::smatch sm;
	bool is_signed = false;
	int mask = 0xffffffff;
	int base = 10;
	if (std::regex_match(numstr, sm, rx)) {
		if (sm[1].matched) {
			auto sub = sm[2];
			width = std::stoi(sub.str());
			mask = (1 << width) - 1;
			sub = sm[3];
			is_signed = sub.str() == "s";
			if (is_signed && width < 2) {
				return boost::wave::T_UNKNOWN;
			}
		}
		if (sm[4].matched) {
			auto sub = sm[4];
			if (sub.str() == "0b" || sub.str() == "0B") {
				base = 2;
			} if (sub.str() == "0d" || sub.str() == "0D") {
				base = 10;
			} if (sub.str() == "0o" || sub.str() == "0O") {
				base = 8;
			} if (sub.str() == "0x" || sub.str() == "0X") {
				base = 16;
			}
		}
		auto sub = sm[5];
		auto str = sub.str();
		std::smatch m;
		if (base == 2) {
			std::regex ex("[01_]+");
			if (!std::regex_match(str, m, ex)) {
				return boost::wave::T_UNKNOWN;
			}
		} else if (base == 8) {
			std::regex ex("[0-7_]+");
			if (!std::regex_match(str, m, ex)) {
				return boost::wave::T_UNKNOWN;
			}
		} else if (base == 10) {
			std::regex ex("[0-9_]+");
			if (!std::regex_match(str, m, ex)) {
				return boost::wave::T_UNKNOWN;
			}
		} else if (base == 16) {
			std::regex ex("[0-9A-Fa-f_]+");
			if (!std::regex_match(str, m, ex)) {
				return boost::wave::T_UNKNOWN;
			}
		}
		value = mask & std::stoi(str, nullptr, base);
		if (is_signed) {
			value = (value & (mask >> 1)) - (value & (1 << (width - 1)));
		}
		return boost::wave::T_NUMBER;
	}
	return boost::wave::T_UNKNOWN;
}

int Lexer::read_pragma()
{
	return boost::wave::T_END_PRAGMA;
}

int Lexer::read_string()
{
	text.clear();
	while (last != '"') {
		if (last == EOF) {
			return boost::wave::T_UNKNOWN;
		}
		if (last == '\\') {
			last = is.get();
			if (last ==  EOF) {
				return boost::wave::T_UNKNOWN;
			}
			text += '\\';
		}
		text += last;
		last = is.get();
	}
	return boost::wave::T_STRING_LITERAL;
}

int Lexer::read_identifier()
{
	text = last;
	while (isalnum((last = is.get())) || last == '_') {
		text += last;
		++col;
	}
	++col;
	if (text == "abstract") {
		return boost::wave::T_ABSTRACT;
	}
	if (text == "action") {
		return boost::wave::T_ACTION;
	}
	if (text == "actions") {
		return boost::wave::T_ACTIONS;
	}
	if (text == "apply") {
		return boost::wave::T_APPLY;
	}
	if (text == "bool") {
		return boost::wave::T_BOOL;
	}
	if (text == "bit") {
		return boost::wave::T_BIT;
	}
	if (text == "const") {
		return boost::wave::T_CONST;
	}
	if (text == "control") {
		return boost::wave::T_CONTROL;
	}
	if (text == "default") {
		return boost::wave::T_DEFAULT;
	}
	if (text == "else") {
		return boost::wave::T_ELSE;
	}
	if (text == "entries") {
		return boost::wave::T_ENTRIES;
	}
	if (text == "enum") {
		return boost::wave::T_ENUM;
	}
	if (text == "error") {
		return boost::wave::T_ERROR;
	}
	if (text == "exit") {
		return boost::wave::T_EXIT;
	}
	if (text == "extern") {
		return boost::wave::T_EXTERN;
	}
	if (text == "false") {
		return boost::wave::T_FALSE;
	}
	if (text == "header") {
		return boost::wave::T_HEADER;
	}
	if (text == "header_union") {
		return boost::wave::T_HEADER_UNION;
	}
	if (text == "if") {
		return boost::wave::T_IF;
	}
	if (text == "in") {
		return boost::wave::T_IN;
	}
	if (text == "inout") {
		return boost::wave::T_INOUT;
	}
	if (text == "int") {
		return boost::wave::T_INT;
	}
	if (text == "key") {
		return boost::wave::T_KEY;
	}
	if (text == "match_kind") {
		return boost::wave::T_MATCH_KIND;
	}
	if (text == "type") {
		return boost::wave::T_TYPE;
	}
	if (text == "out") {
		return boost::wave::T_OUT;
	}
	if (text == "parser") {
		return boost::wave::T_PARSER;
	}
	if (text == "package") {
		return boost::wave::T_PACKAGE;
	}
	if (text == "return") {
		return boost::wave::T_RETURN;
	}
	if (text == "select") {
		return boost::wave::T_SELECT;
	}
	if (text == "state") {
		return boost::wave::T_STATE;
	}
	if (text == "struct") {
		return boost::wave::T_STRUCT;
	}
	if (text == "switch") {
		return boost::wave::T_SWITCH;
	}
	if (text == "table") {
		return boost::wave::T_TABLE;
	}
	if (text == "this") {
		return boost::wave::T_THIS;
	}
	if (text == "transition") {
		return boost::wave::T_TRANSITION;
	}
	if (text == "true") {
		return boost::wave::T_TRUE;
	}
	if (text == "tuple") {
		return boost::wave::T_TUPLE;
	}
	if (text == "typedef") {
		return boost::wave::T_TYPEDEF;
	}
	if (text == "varbit") {
		return boost::wave::T_VARBIT;
	}
	if (text == "value_set") {
		return boost::wave::T_VALUE_SET;
	}
	if (text == "void") {
		return boost::wave::T_VOID;
	}
	if (text == "_") {
		return boost::wave::T_DONTCARE;
	}
	return boost::wave::T_IDENTIFIER;
}

void Lexer::read_comment()
{
	if (last == '/') {
		std::stringbuf sb;
		is.get(sb);
	} else {
		do {
			advance();
			if (last == '*') {
				do {
					advance();
				} while (last == '*');
				if (last == '/') {
					last = '\0';
				}
			}
		} while (last != '\0');
	}
	advance();
}

void Lexer::read_location()
{
	std::stringbuf sb;
	is.get(sb);
	advance();
}

} // namespace p4l

std::ostream& operator<<(std::ostream& os, const boost::wave::token_id& tok)
{
	os << static_cast<int>(tok);
	return os;
}
