/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"

#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include <istream>

class LSP_server : public Protocol {
public:
	LSP_server(std::istream &input_stream, std::ostream &output_stream);
	int run();

private:
	void on_exit(Params_exit &params) override;
	void on_initialize(Params_initialize &params) override;
	void on_shutdown(Params_shutdown &params) override;
	void on_textDocument_codeAction(Params_textDocument_codeAction &params) override;
	void on_textDocument_completion(Params_textDocument_completion &params) override;
	void on_textDocument_definition(Params_textDocument_definition &params) override;
	void on_textDocument_didChange(Params_textDocument_didChange &params) override;
	void on_textDocument_didClose(Params_textDocument_didClose &params) override;
	void on_textDocument_didOpen(Params_textDocument_didOpen &params) override;
	void on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params) override;
	void on_textDocument_formatting(Params_textDocument_formatting &params) override;
	void on_textDocument_hover(Params_textDocument_hover &params) override;
	void on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params) override;
	void on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params) override;
	void on_textDocument_rename(Params_textDocument_rename &params) override;
	void on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params) override;
	void on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params) override;
	void on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params) override;
	void on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params) override;
	void on_workspace_executeCommand(Params_workspace_executeCommand &params) override;

	boost::optional<rapidjson::Document> read_message();

	Server_capabilities _capabilities;
	std::istream &_input_stream;
	std::ostream &_output_stream;
	bool _is_done;
};
