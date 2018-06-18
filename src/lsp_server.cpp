#include "lsp_server.h"
#include "dispatcher.h"
#include "p4unit.h"

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>

#include <functional>
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
	_is_done(false),
	_work(new boost::asio::io_service::work(_io_context)),
	_worker_thread(std::thread([&]{_io_context.run();}))
{
}

int LSP_server::run()
{
	Dispatcher dispatcher([](const rapidjson::Value&) { reply(ERROR_CODES::MethodNotFound, "method not found"); });
	register_protocol_handlers(dispatcher, *this);
	while (!_is_done && _input_stream.good())
	{
		auto message = read_message();
		if (message)
		{
			auto content = *message;
			_io_context.post([this, content, &dispatcher]{dispatcher.call(content, this->_output_stream);});
		}
		else
		{
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(error) << "read_message didn't return a valid json object.";
#endif
		}
	}
	_is_done = true;
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "FINISHED";
#endif
	_work.reset();
	_worker_thread.join();
	return 0;
}

void LSP_server::on_exit(Params_exit &params)
{
	_is_done = true;
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_initialize(Params_initialize &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kObjectType);
	result.AddMember("capabilities", _capabilities.get_json(allocator), allocator);
	reply(result);
}

void LSP_server::on_shutdown(Params_shutdown &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
	rapidjson::Value result(rapidjson::kNullType);
	reply(result);
}

void LSP_server::on_textDocument_codeAction(Params_textDocument_codeAction &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_completion(Params_textDocument_completion &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_definition(Params_textDocument_definition &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_didChange(Params_textDocument_didChange &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_didClose(Params_textDocument_didClose &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_didOpen(Params_textDocument_didOpen &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
	std::string compile_command("/Users/dmakarov/work/try/p4ls/build/ninja/src/tool/p4lsd -I /Users/dmakarov/work/try/p4c/p4include ");
	_files[params._text_document._uri._path] = document_file{params._text_document._text, compile_command + params._text_document._uri._path};
}

void LSP_server::on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_documentSymbol(Params_textDocument_documentSymbol &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
	auto &text = _files[params._text_document._uri._path]._text;
	auto &command = _files[params._text_document._uri._path]._compile_command;
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Create new P4_file with command " << command << " path " << params._text_document._uri._path << " and text.";
#endif
	P4_file file(command, params._text_document._uri._path, text);
	std::vector<Symbol_information> symbols;
	file.get_symbols(symbols);
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kArrayType);
	for (auto& it : symbols)
	{
		result.PushBack(it.get_json(allocator), allocator);
	}
	reply(result);
}

void LSP_server::on_textDocument_formatting(Params_textDocument_formatting &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_hover(Params_textDocument_hover &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_rename(Params_textDocument_rename &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

void LSP_server::on_workspace_executeCommand(Params_workspace_executeCommand &params)
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__;
#endif
}

boost::optional<std::string> LSP_server::read_message()
{
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Start reading a new message";
#endif
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
#if LOGGING_ENABLED
		BOOST_LOG_TRIVIAL(info) << "Current line '" << line << "'";
#endif
		if (!_input_stream.good() && errno == EINTR)
		{
			_input_stream.clear();
			continue;
		}
		if (0 == line.find_first_of('#'))
		{
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Skipping a comment line";
#endif
			continue;
		}
		if (std::regex_match(line, match, content_length_regex))
		{
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Regex match " << match[1].str();
#endif
			content_length = std::stoull(match[1].str());
			continue;
		}
		else if (!line.empty())
		{
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Ignoring another header line";
#endif
			continue;
		}
		else
		{
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Empty line, header ended";
#endif
			break;
		}
	}
	// discard unrealistically large requests
#if LOGGING_ENABLED
	BOOST_LOG_TRIVIAL(info) << "Content length " << content_length;
#endif
	if (content_length > 1 << 30)
	{
#if LOGGING_ENABLED
		BOOST_LOG_TRIVIAL(info) << "Huge message size " << content_length;
#endif
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
#if LOGGING_ENABLED
			BOOST_LOG_TRIVIAL(info) << "Read " << _input_stream.gcount() << " bytes, expected " << content_length << " content <" << content << ">";
#endif
			return boost::none;
		}
#if LOGGING_ENABLED
		BOOST_LOG_TRIVIAL(info) << "Received request " << content;
#endif
		return std::move(content);
	}
	return boost::none;
}
