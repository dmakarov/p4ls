#include "lsp_server.h"

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <regex>
#include <string>

LSP_server::LSP_server() : _is_done(false)
{
}

int LSP_server::run(std::istream &input_stream)
{
	Dispatcher dispatcher;
	register_protocol_handlers(dispatcher, *this);
	while (!_is_done && input_stream.good())
	{
		if (auto json = read_message(input_stream))
		{
			if (!dispatcher.call(*json))
			{
				BOOST_LOG_TRIVIAL(error) << "JSON dispatch failed!";
			}
		}
	}
	_is_done = true;
	return 0;
}

void LSP_server::on_exit(Params_exit &params)
{
	_is_done = true;
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_initialize(Params_initialize &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_shutdown(Params_shutdown &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_codeAction(Params_textDocument_codeAction &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_completion(Params_textDocument_completion &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_definition(Params_textDocument_definition &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_didChange(Params_textDocument_didChange &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_didClose(Params_textDocument_didClose &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_didOpen(Params_textDocument_didOpen &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_formatting(Params_textDocument_formatting &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_hover(Params_textDocument_hover &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_rename(Params_textDocument_rename &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
}

void LSP_server::on_workspace_executeCommand(Params_workspace_executeCommand &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
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
		BOOST_LOG_TRIVIAL(info) << "Current line '" << line << "'";
		if (!input_stream.good() && errno == EINTR)
		{
			input_stream.clear();
			continue;
		}
		if (0 == line.find_first_of('#'))
		{
			BOOST_LOG_TRIVIAL(info) << "Skipping a comment line";
			continue;
		}
		if (std::regex_match(line, match, content_length_regex))
		{
			BOOST_LOG_TRIVIAL(info) << "Regex match " << match[1].str();
			content_length = std::stoull(match[1].str());
			continue;
		}
		else if (!line.empty())
		{
			BOOST_LOG_TRIVIAL(info) << "Ignoring another header line";
			continue;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "Empty line, header ended";
			break;
		}
	}
	// discard unrealistically large requests
	BOOST_LOG_TRIVIAL(info) << "Content length " << content_length;
	if (content_length > 1 << 30)
	{
		BOOST_LOG_TRIVIAL(info) << "Huge message size " << content_length;
		input_stream.ignore(content_length);
		return boost::none;
	}
	// parse JSON payload
	if (content_length > 0)
	{
		std::string content(content_length, '\0');
		input_stream.read(&content[0], content_length);
		if (!input_stream)
		{
			BOOST_LOG_TRIVIAL(info) << "Read " << input_stream.gcount() << " bytes, expected " << content_length;
			return boost::none;
		}
		BOOST_LOG_TRIVIAL(info) << "Received request " << content;
		rapidjson::Document json_document;
		if (json_document.Parse(content.c_str()).HasParseError())
		{
			BOOST_LOG_TRIVIAL(info) << "JSON parse error: " << rapidjson::GetParseError_En(json_document.GetParseError()) << " (" << json_document.GetErrorOffset() << ")";
			return boost::none;
		}
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		json_document.Accept(writer);
		BOOST_LOG_TRIVIAL(info) << "Parsed document " << buffer.GetString();
		return std::move(json_document);
	}
	return boost::none;
}
