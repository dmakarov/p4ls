#include "lexer.h"

#include <regex>
#include <sstream>

namespace p4l {

Token Lexer::next()
{
	while (isspace(last)) {
		advance();
	}
	if (last == EOF) {
		return Token::P4L_END;
	}
	if (isalpha(last) || last == '_') {
		return read_identifier();
	}
	if (isdigit(last)) {
		return read_number();
	}
	return read_other();
}

Token Lexer::read_other()
{
	auto prev = last;
	advance();
	switch (prev) {
	case '!': // "!=" "!"
		if (last == '=') {
			advance();
			return Token::P4L_NE;
		}
		return Token::P4L_NOT;
	case '"': return read_string();
	case '#':
		read_location();
		if (last == EOF) {
			return Token::P4L_END;
		}
		return next();
	case '%': return Token::P4L_MOD;
	case '&': // "&&&" "&&" "&"
		if (last == '&') {
			if (advance() == '&') {
				advance();
				return Token::P4L_MASK;
			}
			return Token::P4L_AND;
		}
		return Token::P4L_BIT_AND;
	case '(': return Token::P4L_L_PAREN;
	case ')': return Token::P4L_R_PAREN;
	case '*': return Token::P4L_MUL;
	case '+': // "++" "+"
		if (last == '+') {
			advance();
			return Token::P4L_PP;
		}
		return Token::P4L_PLUS;
	case ',': return Token::P4L_COMMA;
	case '-': return Token::P4L_MINUS;
	case '.': // ".." "."
		if (last == '.') {
			advance();
			return Token::P4L_RANGE;
		}
		return Token::P4L_DOT;
	case '/': // "//" "/*" "/"
		if (last == '/' || last == '*') {
			read_comment();
			return next();
		}
		return Token::P4L_DIV;
	case ':': return Token::P4L_COLON;
	case ';': return Token::P4L_SEMICOLON;
	case '<': // "<<" "<=" "<"
		if (last == '<') {
			advance();
			return Token::P4L_SHL;
		}
		if (last == '=') {
			advance();
			return Token::P4L_LE;
		}
		return Token::P4L_L_ANGLE;
	case '=': // "==" "="
		if (last == '=') {
			advance();
			return Token::P4L_EQ;
		}
		return Token::P4L_ASSIGN;
	case '>': // ">=" ">"
		if (last == '=') {
			advance();
			return Token::P4L_GE;
		}
		return Token::P4L_R_ANGLE;
	case '?': return Token::P4L_QUESTION;
	case '@':
		if (last == 'p') {
			return read_pragma();
		}
		return Token::P4L_AT;
	case '[': return Token::P4L_L_BRACKET;
	case ']': return Token::P4L_R_BRACKET;
	case '^': return Token::P4L_BIT_XOR;
	case '{': return Token::P4L_L_BRACE;
	case '|': // "|+|" "|-|" "||" "|"
		if (last == '+') {
			if (advance() == '|') {
				advance();
				return Token::P4L_PLUS_SAT;
			}
			return Token::P4L_UNEXPECTED_TOKEN;
		}
		if (last == '-') {
			if (advance() == '|') {
				advance();
				return Token::P4L_MINUS_SAT;
			}
			return Token::P4L_UNEXPECTED_TOKEN;
		}
		if (last == '|') {
			advance();
			return Token::P4L_OR;
		}
		return Token::P4L_BIT_OR;
	case '}': return Token::P4L_R_BRACE;
	case '~': return Token::P4L_COMPLEMENT;
	default:;
	}
	return Token::P4L_UNEXPECTED_TOKEN;
}

Token Lexer::read_number()
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
				return Token::P4L_UNEXPECTED_TOKEN;
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
				return Token::P4L_UNEXPECTED_TOKEN;
			}
		} else if (base == 8) {
			std::regex ex("[0-7_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::P4L_UNEXPECTED_TOKEN;
			}
		} else if (base == 10) {
			std::regex ex("[0-9_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::P4L_UNEXPECTED_TOKEN;
			}
		} else if (base == 16) {
			std::regex ex("[0-9A-Fa-f_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::P4L_UNEXPECTED_TOKEN;
			}
		}
		value = mask & std::stoi(str, nullptr, base);
		if (is_signed) {
			value = (value & (mask >> 1)) - (value & (1 << (width - 1)));
		}
		return Token::P4L_NUMBER;
	}
	return Token::P4L_UNEXPECTED_TOKEN;
}

Token Lexer::read_pragma()
{
	return Token::P4L_END_PRAGMA;
}

Token Lexer::read_string()
{
	text.clear();
	while (last != '"') {
		if (last == EOF) {
			return Token::P4L_UNEXPECTED_TOKEN;
		}
		if (last == '\\') {
			last = is.get();
			if (last ==  EOF) {
				return Token::P4L_UNEXPECTED_TOKEN;
			}
			text += '\\';
		}
		text += last;
		last = is.get();
	}
	return Token::P4L_STRING_LITERAL;
}

Token Lexer::read_identifier()
{
	text = last;
	while (isalnum((last = is.get())) || last == '_') {
		text += last;
		++col;
	}
	++col;
	if (text == "abstract") {
		return Token::P4L_ABSTRACT;
	}
	if (text == "action") {
		return Token::P4L_ACTION;
	}
	if (text == "actions") {
		return Token::P4L_ACTIONS;
	}
	if (text == "apply") {
		return Token::P4L_APPLY;
	}
	if (text == "bool") {
		return Token::P4L_BOOL;
	}
	if (text == "bit") {
		return Token::P4L_BIT;
	}
	if (text == "const") {
		return Token::P4L_CONST;
	}
	if (text == "control") {
		return Token::P4L_CONTROL;
	}
	if (text == "default") {
		return Token::P4L_DEFAULT;
	}
	if (text == "else") {
		return Token::P4L_ELSE;
	}
	if (text == "entries") {
		return Token::P4L_ENTRIES;
	}
	if (text == "enum") {
		return Token::P4L_ENUM;
	}
	if (text == "error") {
		return Token::P4L_ERROR;
	}
	if (text == "exit") {
		return Token::P4L_EXIT;
	}
	if (text == "extern") {
		return Token::P4L_EXTERN;
	}
	if (text == "false") {
		return Token::P4L_FALSE;
	}
	if (text == "header") {
		return Token::P4L_HEADER;
	}
	if (text == "header_union") {
		return Token::P4L_HEADER_UNION;
	}
	if (text == "if") {
		return Token::P4L_IF;
	}
	if (text == "in") {
		return Token::P4L_IN;
	}
	if (text == "inout") {
		return Token::P4L_INOUT;
	}
	if (text == "int") {
		return Token::P4L_INT;
	}
	if (text == "key") {
		return Token::P4L_KEY;
	}
	if (text == "match_kind") {
		return Token::P4L_MATCH_KIND;
	}
	if (text == "type") {
		return Token::P4L_TYPE;
	}
	if (text == "out") {
		return Token::P4L_OUT;
	}
	if (text == "parser") {
		return Token::P4L_PARSER;
	}
	if (text == "package") {
		return Token::P4L_PACKAGE;
	}
	if (text == "return") {
		return Token::P4L_RETURN;
	}
	if (text == "select") {
		return Token::P4L_SELECT;
	}
	if (text == "state") {
		return Token::P4L_STATE;
	}
	if (text == "struct") {
		return Token::P4L_STRUCT;
	}
	if (text == "switch") {
		return Token::P4L_SWITCH;
	}
	if (text == "table") {
		return Token::P4L_TABLE;
	}
	if (text == "this") {
		return Token::P4L_THIS;
	}
	if (text == "transition") {
		return Token::P4L_TRANSITION;
	}
	if (text == "true") {
		return Token::P4L_TRUE;
	}
	if (text == "tuple") {
		return Token::P4L_TUPLE;
	}
	if (text == "typedef") {
		return Token::P4L_TYPEDEF;
	}
	if (text == "varbit") {
		return Token::P4L_VARBIT;
	}
	if (text == "value_set") {
		return Token::P4L_VALUE_SET;
	}
	if (text == "void") {
		return Token::P4L_VOID;
	}
	if (text == "_") {
		return Token::P4L_DONTCARE;
	}
	return Token::P4L_IDENTIFIER;
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

std::ostream& operator<<(std::ostream& os, const p4l::Token& tok)
{
	os << static_cast<int>(tok);
	return os;
}
