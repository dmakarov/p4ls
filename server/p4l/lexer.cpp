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
		return Token::END;
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
			return Token::NE;
		}
		return Token::NOT;
	case '"': return read_string();
	case '#':
		read_location();
		if (last == EOF) {
			return Token::END;
		}
		return next();
	case '%': return Token::MOD;
	case '&': // "&&&" "&&" "&"
		if (last == '&') {
			if (advance() == '&') {
				advance();
				return Token::MASK;
			}
			return Token::AND;
		}
		return Token::BIT_AND;
	case '(': return Token::L_PAREN;
	case ')': return Token::R_PAREN;
	case '*': return Token::MUL;
	case '+': // "++" "+"
		if (last == '+') {
			advance();
			return Token::PP;
		}
		return Token::PLUS;
	case ',': return Token::COMMA;
	case '-': return Token::MINUS;
	case '.': // ".." "."
		if (last == '.') {
			advance();
			return Token::RANGE;
		}
		return Token::DOT;
	case '/': // "//" "/*" "/"
		if (last == '/' || last == '*') {
			read_comment();
			return next();
		}
		return Token::DIV;
	case ':': return Token::COLON;
	case ';': return Token::SEMICOLON;
	case '<': // "<<" "<=" "<"
		if (last == '<') {
			advance();
			return Token::SHL;
		}
		if (last == '=') {
			advance();
			return Token::LE;
		}
		return Token::L_ANGLE;
	case '=': // "==" "="
		if (last == '=') {
			advance();
			return Token::EQ;
		}
		return Token::ASSIGN;
	case '>': // ">=" ">"
		if (last == '=') {
			advance();
			return Token::GE;
		}
		return Token::R_ANGLE;
	case '?': return Token::QUESTION;
	case '@':
		if (last == 'p') {
			return read_pragma();
		}
		return Token::AT;
	case '[': return Token::L_BRACKET;
	case ']': return Token::R_BRACKET;
	case '^': return Token::BIT_XOR;
	case '{': return Token::L_BRACE;
	case '|': // "|+|" "|-|" "||" "|"
		if (last == '+') {
			if (advance() == '|') {
				advance();
				return Token::PLUS_SAT;
			}
			return Token::UNEXPECTED_TOKEN;
		}
		if (last == '-') {
			if (advance() == '|') {
				advance();
				return Token::MINUS_SAT;
			}
			return Token::UNEXPECTED_TOKEN;
		}
		if (last == '|') {
			advance();
			return Token::OR;
		}
		return Token::BIT_OR;
	case '}': return Token::R_BRACE;
	case '~': return Token::COMPLEMENT;
	default:;
	}
	return Token::UNEXPECTED_TOKEN;
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
				return Token::UNEXPECTED_TOKEN;
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
				return Token::UNEXPECTED_TOKEN;
			}
		} else if (base == 8) {
			std::regex ex("[0-7_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::UNEXPECTED_TOKEN;
			}
		} else if (base == 10) {
			std::regex ex("[0-9_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::UNEXPECTED_TOKEN;
			}
		} else if (base == 16) {
			std::regex ex("[0-9A-Fa-f_]+");
			if (!std::regex_match(str, m, ex)) {
				return Token::UNEXPECTED_TOKEN;
			}
		}
		value = mask & std::stoi(str, nullptr, base);
		if (is_signed) {
			value = (value & (mask >> 1)) - (value & (1 << (width - 1)));
		}
		return Token::NUMBER;
	}
	return Token::UNEXPECTED_TOKEN;
}

Token Lexer::read_pragma()
{
	return Token::END_PRAGMA;
}

Token Lexer::read_string()
{
	text.clear();
	while (last != '"') {
		if (last == EOF) {
			return Token::UNEXPECTED_TOKEN;
		}
		if (last == '\\') {
			last = is.get();
			if (last ==  EOF) {
				return Token::UNEXPECTED_TOKEN;
			}
			text += '\\';
		}
		text += last;
		last = is.get();
	}
	return Token::STRING_LITERAL;
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
		return Token::ABSTRACT;
	}
	if (text == "action") {
		return Token::ACTION;
	}
	if (text == "actions") {
		return Token::ACTIONS;
	}
	if (text == "apply") {
		return Token::APPLY;
	}
	if (text == "bool") {
		return Token::BOOL;
	}
	if (text == "bit") {
		return Token::BIT;
	}
	if (text == "const") {
		return Token::CONST;
	}
	if (text == "control") {
		return Token::CONTROL;
	}
	if (text == "default") {
		return Token::DEFAULT;
	}
	if (text == "else") {
		return Token::ELSE;
	}
	if (text == "entries") {
		return Token::ENTRIES;
	}
	if (text == "enum") {
		return Token::ENUM;
	}
	if (text == "error") {
		return Token::ERROR;
	}
	if (text == "exit") {
		return Token::EXIT;
	}
	if (text == "extern") {
		return Token::EXTERN;
	}
	if (text == "false") {
		return Token::FALSE;
	}
	if (text == "header") {
		return Token::HEADER;
	}
	if (text == "header_union") {
		return Token::HEADER_UNION;
	}
	if (text == "if") {
		return Token::IF;
	}
	if (text == "in") {
		return Token::IN;
	}
	if (text == "inout") {
		return Token::INOUT;
	}
	if (text == "int") {
		return Token::INT;
	}
	if (text == "key") {
		return Token::KEY;
	}
	if (text == "match_kind") {
		return Token::MATCH_KIND;
	}
	if (text == "type") {
		return Token::TYPE;
	}
	if (text == "out") {
		return Token::OUT;
	}
	if (text == "parser") {
		return Token::PARSER;
	}
	if (text == "package") {
		return Token::PACKAGE;
	}
	if (text == "return") {
		return Token::RETURN;
	}
	if (text == "select") {
		return Token::SELECT;
	}
	if (text == "state") {
		return Token::STATE;
	}
	if (text == "struct") {
		return Token::STRUCT;
	}
	if (text == "switch") {
		return Token::SWITCH;
	}
	if (text == "table") {
		return Token::TABLE;
	}
	if (text == "this") {
		return Token::THIS;
	}
	if (text == "transition") {
		return Token::TRANSITION;
	}
	if (text == "true") {
		return Token::TRUE;
	}
	if (text == "tuple") {
		return Token::TUPLE;
	}
	if (text == "typedef") {
		return Token::TYPEDEF;
	}
	if (text == "varbit") {
		return Token::VARBIT;
	}
	if (text == "value_set") {
		return Token::VALUE_SET;
	}
	if (text == "void") {
		return Token::VOID;
	}
	if (text == "_") {
		return Token::DONTCARE;
	}
	return Token::IDENTIFIER;
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
