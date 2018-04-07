#include "lsp_server.h"

#include <rapidjson/document.h>

#include <iostream>

LSP_server::LSP_server() : _is_done(false)
{
}

int LSP_server::run(std::istream &input_stream)
{
	while (!_is_done && input_stream.good()) {
		if (auto json = read_message(input_stream)) {
		}
	}
	return 0;
}

void LSP_server::on_exit(Params_exit &params)
{
}

void LSP_server::on_initialize(Params_initialize &params)
{
}

void LSP_server::on_shutdown(Params_shutdown &params)
{
}

void LSP_server::on_textDocument_codeAction(Params_textDocument_codeAction &params)
{
}

void LSP_server::on_textDocument_completion(Params_textDocument_completion &params)
{
}

void LSP_server::on_textDocument_definition(Params_textDocument_definition &params)
{
}

void LSP_server::on_textDocument_didChange(Params_textDocument_didChange &params)
{
}

void LSP_server::on_textDocument_didClose(Params_textDocument_didClose &params)
{
}

void LSP_server::on_textDocument_didOpen(Params_textDocument_didOpen &params)
{
}

void LSP_server::on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params)
{
}

void LSP_server::on_textDocument_formatting(Params_textDocument_formatting &params)
{
}

void LSP_server::on_textDocument_hover(Params_textDocument_hover &params)
{
}

void LSP_server::on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params)
{
}

void LSP_server::on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params)
{
}

void LSP_server::on_textDocument_rename(Params_textDocument_rename &params)
{
}

void LSP_server::on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params)
{
}

void LSP_server::on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params)
{
}

void LSP_server::on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params)
{
}

void LSP_server::on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params)
{
}

void LSP_server::on_workspace_executeCommand(Params_workspace_executeCommand &params)
{
}


boost::optional<rapidjson::Document> LSP_server::read_message(std::istream &input_stream)
{
	std::string json_input;
	input_stream >> json_input;
	rapidjson::Document json_document;
	std::cout << "Input is " << json_input << std::endl;
	json_document.Parse(json_input.c_str());
	for (auto& m : json_document.GetObject())
		std::cout << "Type of member " << m.name.GetString()
				  << " is " << m.value.GetType() << std::endl;
	return json_document;
}
