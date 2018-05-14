#include "protocol.h"

#include <boost/log/trivial.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <sstream>

std::ostream &operator<<(std::ostream &os, const URI &item)
{
	os << item._path;
	return os;
}

rapidjson::Value Server_capabilities::Text_document_sync_options::get_json(rapidjson::Document::AllocatorType &allocator)
{
	rapidjson::Value result(rapidjson::kObjectType);
	if (open_close)
	{
		result.AddMember("openClose", *open_close, allocator);
	}
	if (will_save)
	{
		result.AddMember("willSave", *will_save, allocator);
	}
	if (will_save_wait_until)
	{
		result.AddMember("willSaveWaitUntil", *will_save_wait_until, allocator);
	}
	if (change)
	{
		result.AddMember("change", static_cast<int>(*change), allocator);
	}
	if (save)
	{
		result.AddMember("save", save->get_json(allocator), allocator);
	}
	return result;
}

rapidjson::Value Server_capabilities::get_json(rapidjson::Document::AllocatorType &allocator)
{
	rapidjson::Value result(rapidjson::kObjectType);
	if (workspace)
	{
		result.AddMember("workspace", workspace->get_json(allocator), allocator);
	}
	if (text_document_sync)
	{
		result.AddMember("textDocumentSync", text_document_sync->get_json(allocator), allocator);
	}
	if (completion_provider)
	{
		result.AddMember("completionProvider", completion_provider->get_json(allocator), allocator);
	}
	if (signature_help_provider)
	{
		result.AddMember("signatureHelpProvider", signature_help_provider->get_json(allocator), allocator);
	}
	if (code_lens_provider)
	{
		result.AddMember("codeLensProvider", code_lens_provider->get_json(allocator), allocator);
	}
	if (document_on_type_formatting_provider)
	{
		result.AddMember("documentOnTypeFormattingProvider", document_on_type_formatting_provider->get_json(allocator), allocator);
	}
	if (document_link_provider)
	{
		result.AddMember("documentLinkProvider", document_link_provider->get_json(allocator), allocator);
	}
	if (color_provider)
	{
		result.AddMember("colorProvider", color_provider->get_json(allocator), allocator);
	}
	if (execute_command_provider)
	{
		result.AddMember("executeCommandProvider", execute_command_provider->get_json(allocator), allocator);
	}
	if (hover_provider)
	{
		result.AddMember("hoverProvider", *hover_provider, allocator);
	}
	if (definition_provider)
	{
		result.AddMember("definitionProvider", *definition_provider, allocator);
	}
	if (type_definition_provider)
	{
		result.AddMember("typeDefinitionProvider", *type_definition_provider, allocator);
	}
	if (implementation_provider)
	{
		result.AddMember("implementationProvider", *implementation_provider, allocator);
	}
	if (references_provider)
	{
		result.AddMember("referencesProvider", *references_provider, allocator);
	}
	if (document_highlight_provider)
	{
		result.AddMember("documentHighlightProvider", *document_highlight_provider, allocator);
	}
	if (document_symbol_provider)
	{
		result.AddMember("documentSymbolProvider", *document_symbol_provider, allocator);
	}
	if (workspace_symbol_provider)
	{
		result.AddMember("workspaceSymbolProvider", *workspace_symbol_provider, allocator);
	}
	if (code_action_provider)
	{
		result.AddMember("codeActionProvider", *code_action_provider, allocator);
	}
	if (document_formatting_provider)
	{
		result.AddMember("documentFormattingProvider", *document_formatting_provider, allocator);
	}
	if (document_range_formatting_provider)
	{
		result.AddMember("documentRangeFormattingProvider", *document_range_formatting_provider, allocator);
	}
	if (rename_provider)
	{
		result.AddMember("renameProvider", *rename_provider, allocator);
	}
	return result;
}


bool set_params_from_json(const rapidjson::Value &json, Params_exit &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_initialize &params)
{
	BOOST_LOG_TRIVIAL(info) << "Start processing params for initialize request";
	if (json.HasMember("processId") && !json["processId"].IsNull())
	{
		params.process_id = json["processId"].GetInt();
	}
	if (json.HasMember("rootUri") && !json["rootUri"].IsNull())
	{
		params.root_uri.set_from_uri(json["rootUri"].GetString());
	}
	else if (json.HasMember("rootPath") && !json["rootPath"].IsNull())
	{
		params.root_uri.set_from_path(json["rootPath"].GetString());
	}
	if (json.HasMember("initializationOptions") && !json["initializationOptions"].IsNull())
	{
		std::vector<std::string> options;
		for (auto *it = json["initializationOptions"].Begin(); it != json["initializationOptions"].End(); ++it)
		{
			options.emplace_back(it->GetString());
		}
		params.initialization_options.emplace(options);
	}
	if (json.HasMember("capabilities"))
	{
		params.capabilities.set(json["capabilities"]);
	}
	if (json.HasMember("trace") && !json["trace"].IsNull())
	{
		params.trace.emplace(
			(json["trace"] == "off"     ) ? Trace_level::OFF :
			(json["trace"] == "messages") ? Trace_level::MESSAGES :
			(json["trace"] == "verbose" ) ? Trace_level::VERBOSE : Trace_level::OFF);
	}
	if (json.HasMember("workspaceFolders") && !json["workspaceFolders"].IsNull())
	{
		std::vector<Workspace_folder> folders;
		for (auto *it = json["workspaceFolders"].Begin(); it != json["workspaceFolders"].End(); ++it)
		{
			folders.emplace_back(Workspace_folder(*it));
		}
		params.workspace_folders.emplace(folders);
	}
	BOOST_LOG_TRIVIAL(info) << "Finish processing params for initialize request";
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_shutdown &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_codeAction &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_completion &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_definition &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didChange &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didClose &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didOpen &params)
{
	auto result = false;
	BOOST_LOG_TRIVIAL(info) << "Start processing params for textDocument/didOpen notification";
	if (json.HasMember("textDocument"))
	{
		result = params.text_document.set(json["textDocument"]);
	}
	BOOST_LOG_TRIVIAL(info) << "Finish processing params for textDocument/didOpen notification " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_documentHighlight &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_documentSymbol &params)
{
	auto result = false;
	BOOST_LOG_TRIVIAL(info) << "Start processing params for textDocument/documentSymbol request";
	if (json.HasMember("textDocument"))
	{
		result = params.text_document.set(json["textDocument"]);
	}
	BOOST_LOG_TRIVIAL(info) << "Finish processing params for textDocument/documentSymbol request " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_formatting &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_hover &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_onTypeFormatting &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rangeFormatting &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rename &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_signatureHelp &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_switchSourceHeader &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeConfiguration &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeWatchedFiles &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_executeCommand &params)
{
	return true;
}
