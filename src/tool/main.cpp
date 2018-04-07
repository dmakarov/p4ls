#include "lsp_server.h"

#include <iostream>

int main(int argc, char* argv[])
{
	LSP_server the_server;
	return the_server.run(std::cin);
}
