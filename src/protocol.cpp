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
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_documentHighlight &params)
{
	return true;
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


namespace
{
	struct registration_helper {
		template <typename param> void operator()(const std::string &method, void(Protocol::*handler)(param))
		{
			auto *protocol = this->protocol;
			dispatcher.register_handler(method, [=](const rapidjson::Value &json) {
				typename std::remove_reference<param>::type params;
				if (set_params_from_json(json, params))
				{
					(protocol->*handler)(params);
				}
				else
				{
					BOOST_LOG_TRIVIAL(error) << method << " cannot be handled.";
				}
			});
		}

		Dispatcher &dispatcher;
		Protocol *protocol;
	};
}

void register_protocol_handlers(Dispatcher &dispatcher, Protocol &protocol)
{
	registration_helper register_handler{dispatcher, &protocol};
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
	register_handler("exit", &Protocol::on_exit);
	register_handler("initialize", &Protocol::on_initialize);
	register_handler("shutdown", &Protocol::on_shutdown);
	register_handler("textDocument/codeAction", &Protocol::on_textDocument_codeAction);
	register_handler("textDocument/completion", &Protocol::on_textDocument_completion);
	register_handler("textDocument/definition", &Protocol::on_textDocument_definition);
	register_handler("textDocument/didChange", &Protocol::on_textDocument_didChange);
	register_handler("textDocument/didClose", &Protocol::on_textDocument_didClose);
	register_handler("textDocument/didOpen", &Protocol::on_textDocument_didOpen);
	register_handler("textDocument/documentHighlight", &Protocol::on_textDocument_documentHighlight);
	register_handler("textDocument/formatting", &Protocol::on_textDocument_formatting);
	register_handler("textDocument/hover", &Protocol::on_textDocument_hover);
	register_handler("textDocument/onTypeFormatting", &Protocol::on_textDocument_onTypeFormatting);
	register_handler("textDocument/rangeFormatting", &Protocol::on_textDocument_rangeFormatting);
	register_handler("textDocument/rename", &Protocol::on_textDocument_rename);
	register_handler("textDocument/signatureHelp", &Protocol::on_textDocument_signatureHelp);
	register_handler("textDocument/switchSourceHeader", &Protocol::on_textDocument_switchSourceHeader);
	register_handler("workspace/didChangeConfiguration", &Protocol::on_workspace_didChangeConfiguration);
	register_handler("workspace/didChangeWatchedFiles", &Protocol::on_workspace_didChangeWatchedFiles);
	register_handler("workspace/executeCommand", &Protocol::on_workspace_executeCommand);
}
