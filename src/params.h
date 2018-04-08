/*
 * -*- c++ -*-
 */

#pragma once

#include <rapidjson/document.h>
#include <boost/optional.hpp>

#include <string>
#include <vector>

enum class Trace_level {
	OFF = 0, MESSAGES = 1, VERBOSE = 2
};

struct File_uri {
};

struct Workspace_folder {
};

struct Client_capabilities {
};

struct Params_exit {
};

bool set_params_from_json(const rapidjson::Value &json, Params_exit &params);

struct Params_initialize {
	boost::optional<int> process_id;
	boost::optional<std::string> root_path;
	boost::optional<File_uri> root_uri;
	Client_capabilities capabilities;
	Trace_level trace;
	std::vector<Workspace_folder> workspace_folders;
};

bool set_params_from_json(const rapidjson::Value &json, Params_initialize &params);

struct Params_shutdown {
};

bool set_params_from_json(const rapidjson::Value &json, Params_shutdown &params);

struct Params_textDocument_codeAction {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_codeAction &params);

struct Params_textDocument_completion {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_completion &params);

struct Params_textDocument_definition {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_definition &params);

struct Params_textDocument_didChange {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didChange &params);

struct Params_textDocument_didClose {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didClose &params);

struct Params_textDocument_didOpen {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didOpen &params);

struct Params_textDocument_documentHighlight {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_documentHighlight &params);

struct Params_textDocument_formatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_formatting &params);

struct Params_textDocument_hover {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_hover &params);

struct Params_textDocument_onTypeFormatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_onTypeFormatting &params);

struct Params_textDocument_rangeFormatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rangeFormatting &params);

struct Params_textDocument_rename {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rename &params);

struct Params_textDocument_signatureHelp {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_signatureHelp &params);

struct Params_textDocument_switchSourceHeader {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_switchSourceHeader &params);

struct Params_workspace_didChangeConfiguration {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeConfiguration &params);

struct Params_workspace_didChangeWatchedFiles {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeWatchedFiles &params);

struct Params_workspace_executeCommand {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_executeCommand &params);
