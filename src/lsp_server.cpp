#include "lsp_server.h"
#include "dispatcher.h"

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <regex>
#include <string>

LSP_server::LSP_server(std::istream &input_stream, std::ostream &output_stream)
	:
	_capabilities {
				   boost::none,
				   Server_capabilities::Text_document_sync_options
				   {
					true, true, true,
					Server_capabilities::Text_document_sync_options::Save_options{true},
					TEXT_DOCUMENT_SYNC_KIND::Incremental
				   },
				   boost::none,
				   boost::none,
				   Server_capabilities::Code_lens_options{true},
				   boost::none,
				   Server_capabilities::Document_link_options{true},
				   boost::none,
				   boost::none,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true,
				   true},
	_input_stream(input_stream),
	_output_stream(output_stream),
	_is_done(false)
{
}

int LSP_server::run()
{
	Dispatcher dispatcher([](const rapidjson::Value&) { reply(ERROR_CODES::MethodNotFound, "method not found"); });
	register_protocol_handlers(dispatcher, *this);
	while (!_is_done && _input_stream.good())
	{
		if (auto json = read_message())
		{
			if (!dispatcher.call(*json, _output_stream))
			{
				BOOST_LOG_TRIVIAL(error) << "JSON dispatch failed!";
			}
		}
	}
	_is_done = true;
	BOOST_LOG_TRIVIAL(info) << "FINISHED";
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
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kObjectType);
	result.AddMember("capabilities", _capabilities.get_json(allocator), allocator);
	reply(result);
}

void LSP_server::on_shutdown(Params_shutdown &params)
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
	rapidjson::Value result(rapidjson::kObjectType);
	reply(result);
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

boost::optional<rapidjson::Document> LSP_server::read_message()
{
	BOOST_LOG_TRIVIAL(info) << "Start reading a new message";
	// process a set of HTTP headers of an LSP message
	unsigned long long content_length = 0;
	std::regex content_length_regex("Content-Length: ([0-9]+)", std::regex::extended);
	std::smatch match;
	while (_input_stream.good())
	{
		std::string line;
		std::getline(_input_stream, line);
		if (line.back() == '\r')
		{
			line.pop_back();
		}
		BOOST_LOG_TRIVIAL(info) << "Current line '" << line << "'";
		if (!_input_stream.good() && errno == EINTR)
		{
			_input_stream.clear();
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
		_input_stream.ignore(content_length);
		return boost::none;
	}
	// parse JSON payload
	if (content_length > 0)
	{
		std::string content(content_length, '\0');
		_input_stream.read(&content[0], content_length);
		if (!_input_stream)
		{
			BOOST_LOG_TRIVIAL(info) << "Read " << _input_stream.gcount() << " bytes, expected " << content_length;
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
