/*
 * -*- c++ -*-
 */

#pragma once

#include "protocol.h"
#include "p4unit.h"

#include <boost/asio.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include <istream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>


class LSP_server : public Protocol {
public:
	static boost::log::sources::severity_logger<int> _logger;
	LSP_server(std::istream& input_stream, std::ostream& output_stream);
	int run();

private:
	void on_exit(Params_exit& params) override;
	void on_initialize(Params_initialize& params) override;
	void on_shutdown(Params_shutdown& params) override;
	void on_textDocument_codeAction(Params_textDocument_codeAction& params) override;
	void on_textDocument_codeLens(Params_textDocument_codeLens& params) override;
	void on_codeLens_resolve(Params_codeLens_resolve& params) override;
	void on_textDocument_completion(Params_textDocument_completion& params) override;
	void on_textDocument_definition(Params_text_document_position& params) override;
	void on_textDocument_didChange(Params_textDocument_didChange& params) override;
	void on_textDocument_didClose(Params_textDocument_didClose& params) override;
	void on_textDocument_didOpen(Params_textDocument_didOpen& params) override;
	void on_textDocument_didSave(Params_textDocument_didSave& params) override;
	void on_textDocument_documentHighlight(Params_text_document_position& params) override;
	void on_textDocument_documentSymbol(Params_textDocument_documentSymbol& params) override;
	void on_textDocument_formatting(Params_textDocument_formatting& params) override;
	void on_textDocument_hover(Params_text_document_position& params) override;
	void on_textDocument_implementation(Params_text_document_position& params) override;
	void on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting& params) override;
	void on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting& params) override;
	void on_textDocument_rename(Params_textDocument_rename& params) override;
	void on_textDocument_signatureHelp(Params_text_document_position& params) override;
	void on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader& params) override;
	void on_textDocument_typeDefinition(Params_text_document_position& params) override;
	void on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration& params) override;
	void on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles& params) override;
	void on_workspace_executeCommand(Params_workspace_executeCommand& params) override;

	boost::optional<std::string> read_message();
	std::string find_command_for_path(std::string& file);

	Server_capabilities _capabilities;
	std::istream& _input_stream;
	std::ostream& _output_stream;
	bool _is_done;

	boost::asio::io_context _io_context;
	std::shared_ptr<boost::asio::io_service::work> _work;
	std::thread _worker_thread;

	std::unordered_map<std::string, P4_file> _files;
	std::unordered_map<std::string, std::string> _commands;
};
