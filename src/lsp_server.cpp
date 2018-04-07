#include "lsp_server.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

#include <iostream>
#include <regex>
#include <string>

LSP_server::LSP_server() : _is_done(false)
{
}

int LSP_server::run(std::istream &input_stream)
{
	Dispatcher dispatcher;
	while (!_is_done && input_stream.good()) {
		if (auto json = read_message(input_stream)) {
			if (!dispatcher.call(*json))
				std::cout << "JSON dispatch failed!" << std::endl;
		}
	}
	_is_done = true;
	return 0;
}

void LSP_server::on_exit(Params_exit &params)
{
	_is_done = true;
	std::cout << "LSP_server::on_exit()" << std::endl;
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
	// process a set of HTTP headers of an LSP message
	unsigned long long content_length = 0;
	std::regex content_length_regex("Content-Length: ([0-9]+)", std::regex::extended);
	std::smatch match;
	while (input_stream.good())
	{
		std::string line;
		std::getline(input_stream, line);
		if (line.back() == '\r')
		{
			line.pop_back();
		}
		std::cout << "Current line '" << line << "'" << std::endl;
		if (!input_stream.good() && errno == EINTR)
		{
			input_stream.clear();
			continue;
		}
		if (0 == line.find_first_of('#'))
		{
			std::cout << "Skipping a comment line" << std::endl;
			continue;
		}
		if (std::regex_match(line, match, content_length_regex))
		{
			std::cout << "Regex match " << match[1].str() << std::endl;
			content_length = std::stoull(match[1].str());
			continue;
		}
		else if (!line.empty())
		{
			std::cout << "Ignoring another header line" << std::endl;
			continue;
		}
		else
		{
			std::cout << "Empty line, header ended" << std::endl;
			break;
		}
	}
	std::cout << "Content length " << content_length << std::endl;
	if (content_length > 1 << 30)
	{
		input_stream.ignore(content_length);
		return boost::none;
	}
	if (content_length > 0)
	{
		rapidjson::IStreamWrapper input_stream_wrapper(input_stream);
		rapidjson::Document json_document;
		if (json_document.ParseStream(input_stream_wrapper).HasParseError())
		{
			std::cout << "JSON parse error: " << rapidjson::GetParseError_En(json_document.GetParseError())
					  << " (" << json_document.GetErrorOffset() << ")" << std::endl;
			return boost::none;
		}
		for (auto& m : json_document.GetObject())
			std::cout << "Type of member " << m.name.GetString()
					  << " is " << m.value.GetType() << std::endl;
		return std::move(json_document);
	}
	return boost::none;
}
