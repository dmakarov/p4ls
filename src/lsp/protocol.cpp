#include "protocol.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <sstream>

boost::log::sources::severity_logger<int> Protocol::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

std::ostream &operator<<(std::ostream &os, const URI &item)
{
	os << item._path;
	return os;
}

std::ostream &operator<<(std::ostream &os, const Range &range)
{
	os << range._start._line << ":" << range._start._character << "-"
	   << range._end._line << ":" << range._end._character;
	return os;
}

bool operator<(const Range& lhs, const Range& rhs)
{
	return lhs._start._line < rhs._start._line
		|| (lhs._start._line == rhs._start._line
			&& lhs._start._character < rhs._start._character);
}

bool operator&(const Range& lhs, const Range& rhs)
{
	return lhs._start._line == rhs._start._line
		&& lhs._start._character <= rhs._start._character
		&& lhs._end._line >= rhs._end._line
		&& lhs._end._character >= rhs._start._character;
}

std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << location._uri
	   << "#" << location._range._start._line << ":" << location._range._start._character
	   << "-" << location._range._end._line << ":" << location._range._end._character;
	return os;
}

bool operator==(const Location& lhs, const Location& rhs)
{
	return lhs._uri == rhs._uri
		&& lhs._range._start._line == rhs._range._start._line
		&& lhs._range._start._character == rhs._range._start._character
		&& lhs._range._end._line == rhs._range._end._line
		&& lhs._range._end._character == rhs._range._end._character;
}

bool operator<(const Location& lhs, const Location& rhs)
{
	return lhs._uri < rhs._uri
		|| (lhs._uri == rhs._uri
			&& (lhs._range._start._line < rhs._range._start._line
				|| (lhs._range._start._line == rhs._range._start._line
					&& lhs._range._start._character < rhs._range._start._character)));
}

rapidjson::Value Server_capabilities::Text_document_sync_options::get_json(rapidjson::Document::AllocatorType &allocator)
{
	rapidjson::Value result(rapidjson::kObjectType);
	if (_open_close)
	{
		result.AddMember("openClose", *_open_close, allocator);
	}
	if (_will_save)
	{
		result.AddMember("willSave", *_will_save, allocator);
	}
	if (_will_save_wait_until)
	{
		result.AddMember("willSaveWaitUntil", *_will_save_wait_until, allocator);
	}
	if (_change)
	{
		result.AddMember("change", static_cast<int>(*_change), allocator);
	}
	if (_save)
	{
		result.AddMember("save", _save->get_json(allocator), allocator);
	}
	return result;
}

rapidjson::Value Server_capabilities::get_json(rapidjson::Document::AllocatorType &allocator)
{
	rapidjson::Value result(rapidjson::kObjectType);
	if (_workspace)
	{
		result.AddMember("workspace", _workspace->get_json(allocator), allocator);
	}
	if (_text_document_sync)
	{
		result.AddMember("textDocumentSync", _text_document_sync->get_json(allocator), allocator);
	}
	if (_completion_provider)
	{
		result.AddMember("completionProvider", _completion_provider->get_json(allocator), allocator);
	}
	if (_signature_help_provider)
	{
		result.AddMember("signatureHelpProvider", _signature_help_provider->get_json(allocator), allocator);
	}
	if (_code_lens_provider)
	{
		result.AddMember("codeLensProvider", _code_lens_provider->get_json(allocator), allocator);
	}
	if (_document_on_type_formatting_provider)
	{
		result.AddMember("documentOnTypeFormattingProvider", _document_on_type_formatting_provider->get_json(allocator), allocator);
	}
	if (_document_link_provider)
	{
		result.AddMember("documentLinkProvider", _document_link_provider->get_json(allocator), allocator);
	}
	if (_color_provider)
	{
		result.AddMember("colorProvider", _color_provider->get_json(allocator), allocator);
	}
	if (_execute_command_provider)
	{
		result.AddMember("executeCommandProvider", _execute_command_provider->get_json(allocator), allocator);
	}
	if (_hover_provider)
	{
		result.AddMember("hoverProvider", *_hover_provider, allocator);
	}
	if (_definition_provider)
	{
		result.AddMember("definitionProvider", *_definition_provider, allocator);
	}
	if (_type_definition_provider)
	{
		result.AddMember("typeDefinitionProvider", *_type_definition_provider, allocator);
	}
	if (_implementation_provider)
	{
		result.AddMember("implementationProvider", *_implementation_provider, allocator);
	}
	if (_references_provider)
	{
		result.AddMember("referencesProvider", *_references_provider, allocator);
	}
	if (_document_highlight_provider)
	{
		result.AddMember("documentHighlightProvider", *_document_highlight_provider, allocator);
	}
	if (_document_symbol_provider)
	{
		result.AddMember("documentSymbolProvider", *_document_symbol_provider, allocator);
	}
	if (_workspace_symbol_provider)
	{
		result.AddMember("workspaceSymbolProvider", *_workspace_symbol_provider, allocator);
	}
	if (_code_action_provider)
	{
		result.AddMember("codeActionProvider", *_code_action_provider, allocator);
	}
	if (_document_formatting_provider)
	{
		result.AddMember("documentFormattingProvider", *_document_formatting_provider, allocator);
	}
	if (_document_range_formatting_provider)
	{
		result.AddMember("documentRangeFormattingProvider", *_document_range_formatting_provider, allocator);
	}
	if (_rename_provider)
	{
		result.AddMember("renameProvider", *_rename_provider, allocator);
	}
	return result;
}

bool set_params_from_json(const rapidjson::Value&, Params_exit&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_initialize &params)
{
	BOOST_LOG(Protocol::_logger) << "processing params for method \"initialize\"";
	if (json.HasMember("processId") && !json["processId"].IsNull())
	{
		params._process_id = json["processId"].GetInt();
	}
	if (json.HasMember("rootUri") && !json["rootUri"].IsNull())
	{
		params._root_uri.set_from_uri(json["rootUri"].GetString());
	}
	else if (json.HasMember("rootPath") && !json["rootPath"].IsNull())
	{
		params._root_uri.set_from_path(json["rootPath"].GetString());
	}
	if (json.HasMember("initializationOptions") && !json["initializationOptions"].IsNull())
	{
		std::vector<std::string> options;
		for (auto *it = json["initializationOptions"].Begin(); it != json["initializationOptions"].End(); ++it)
		{
			options.emplace_back(it->GetString());
		}
		params._initialization_options.emplace(options);
	}
	if (json.HasMember("capabilities"))
	{
		params._capabilities.set(json["capabilities"]);
	}
	if (json.HasMember("trace") && !json["trace"].IsNull())
	{
		params._trace.emplace(
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
		params._workspace_folders.emplace(folders);
	}
	BOOST_LOG(Protocol::_logger) << "processed  params for method \"initialize\"";
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_shutdown&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value& json, Params_text_document_position& params)
{
	BOOST_LOG(Protocol::_logger) << "processing \"TextDocumentPositionParams\"";
	auto result = params.set(json);
	BOOST_LOG(Protocol::_logger) << "processed  \"TextDocumentPositionParams\" " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_codeAction&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_codeLens&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_codeLens_resolve&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_completion&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didChange& params)
{
	BOOST_LOG(Protocol::_logger) << "processing params for method \"textDocument/didChange\"";
	auto result = params.set(json);
	BOOST_LOG(Protocol::_logger) << "processed  params for method \"textDocument/didChange\" " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_didClose&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didOpen& params)
{
	auto result = false;
	BOOST_LOG(Protocol::_logger) << "processing params for method \"textDocument/didOpen\"";
	if (json.HasMember("textDocument"))
	{
		result = params._text_document.set(json["textDocument"]);
	}
	BOOST_LOG(Protocol::_logger) << "processed  params for method \"textDocument/didOpen\" " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didSave& params)
{
	auto result = false;
	BOOST_LOG(Protocol::_logger) << "processing params for method \"textDocument/didSave\"";
	if (json.HasMember("textDocument"))
	{
		result = params._text_document.set(json["textDocument"]);
	}
	if (json.HasMember("text"))
	{
		params._text.emplace(json["text"].GetString());
	}
	BOOST_LOG(Protocol::_logger) << "processed  params for method \"textDocument/didSave\" " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_documentSymbol& params)
{
	auto result = false;
	BOOST_LOG(Protocol::_logger) << "processing params for method \"textDocument/documentSymbol\"";
	if (json.HasMember("textDocument"))
	{
		result = params._text_document.set(json["textDocument"]);
	}
	BOOST_LOG(Protocol::_logger) << "processed  params for method \"textDocument/documentSymbol\" " << result;
	return result;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_formatting&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_onTypeFormatting&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_rangeFormatting&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_rename&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_textDocument_switchSourceHeader&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_workspace_didChangeConfiguration&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_workspace_didChangeWatchedFiles&)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value&, Params_workspace_executeCommand&)
{
	return true;
}
