#include "parser.h"


namespace p4l {

void Parser::run()
{
	while (true) {
		switch (token) {
		case Token::END: return;
		default:;
		}
	}
}

} // namespace p4l
