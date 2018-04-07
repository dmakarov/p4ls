/*
 * -*- c++ -*-
 */

#pragma once

#include "dispatcher.h"
#include "params.h"

class Protocol {
public:
	virtual ~Protocol() = default;

	virtual void on_exit(Params_exit &params) = 0;
	virtual void on_initialize(Params_initialize &params) = 0;
	virtual void on_shutdown(Params_shutdown &params) = 0;
	virtual void on_textDocument_codeAction(Params_textDocument_codeAction &params) = 0;
	virtual void on_textDocument_completion(Params_textDocument_completion &params) = 0;
	virtual void on_textDocument_definition(Params_textDocument_definition &params) = 0;
	virtual void on_textDocument_didChange(Params_textDocument_didChange &params) = 0;
	virtual void on_textDocument_didClose(Params_textDocument_didClose &params) = 0;
	virtual void on_textDocument_didOpen(Params_textDocument_didOpen &params) = 0;
	virtual void on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params) = 0;
	virtual void on_textDocument_formatting(Params_textDocument_formatting &params) = 0;
	virtual void on_textDocument_hover(Params_textDocument_hover &params) = 0;
	virtual void on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params) = 0;
	virtual void on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params) = 0;
	virtual void on_textDocument_rename(Params_textDocument_rename &params) = 0;
	virtual void on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params) = 0;
	virtual void on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params) = 0;
	virtual void on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params) = 0;
	virtual void on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params) = 0;
	virtual void on_workspace_executeCommand(Params_workspace_executeCommand &params) = 0;
};

void register_protocol_handlers(Dispatcher &dispatcher, Protocol &protocol);
