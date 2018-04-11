#include "params.h"

#include <boost/log/trivial.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <sstream>

bool set_params_from_json(const rapidjson::Value &json, Params_exit &params)
{
	return true;
}

bool set_params_from_json(const rapidjson::Value &json, Params_initialize &params)
{
	if (json.HasMember("processId"))
	{
		params.process_id = json["processId"].GetInt();
	}
	if (json.HasMember("rootUri"))
	{
		params.root_uri.set_from_uri(json["rootUri"].GetString());
	}
	else if (json.HasMember("rootPath"))
	{
		params.root_uri.set_from_path(json["rootPath"].GetString());
	}
	if (json.HasMember("initializationOptions"))
	{
		params.options = json["initializationOptions"].GetString();
	}
	if (json.HasMember("capabilities"))
	{
		params.capabilities.set(json["capabilities"]);
	}
	if (json.HasMember("trace"))
	{
		if (json["trace"] == "off")
		{
			params.trace = Trace_level::OFF;
		} else if (json["trace"] == "messages")
		{
			params.trace = Trace_level::MESSAGES;
		}
		else if (json["trace"] == "verbose")
		{
			params.trace = Trace_level::VERBOSE;
		}
	}
	if (json.HasMember("workspaceFolders"))
	{
		for (auto *it = json["workspaceFolders"].Begin(); it != json["workspaceFolders"].End(); ++it)
		{
			params.workspace_folders.emplace_back(*it);
		}
	}
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
