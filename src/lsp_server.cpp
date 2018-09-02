#include "lsp_server.h"
#include "dispatcher.h"

#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <fstream>
#include <functional>
#include <regex>
#include <string>


boost::log::sources::severity_logger<int> LSP_server::_logger(boost::log::keywords::severity = boost::log::sinks::syslog::debug);

LSP_server::LSP_server(std::istream &input_stream, std::ostream &output_stream)
	: _capabilities {
					 boost::none, // Workspace specific server capabilities
					 Server_capabilities::Text_document_sync_options {
					  true,       // open and close notifications are sent to the server
					  true,       // will save notifications are sent to the server
					  true,       // will save wait until requests are sent to the server
					  Server_capabilities::Text_document_sync_options::Save_options {
					   true       // the client is supposed to include the content on save
					  },
					  TEXT_DOCUMENT_SYNC_KIND::Incremental
					 },           // how text documents are synced
					 boost::none, // completion support
					 boost::none, // signature help
					 boost::none, // code lens
					 boost::none, // document formatting on typing
					 boost::none, // document link support
					 boost::none, // color provider
					 boost::none, // execute command
					 true,        // hover support
					 true,        // go to definition
					 true,        // go to type definition
					 true,        // go to implementation
					 true,        // find references
					 true,        // highlight support
					 true,        // document symbol
					 boost::none, // workspace symbol
					 boost::none, // code actions
					 boost::none, // formatting
					 boost::none, // range formatting
					 true         // rename support
      }
	, _input_stream(input_stream)
	, _output_stream(output_stream)
	, _is_done(false)
	, _work(new boost::asio::io_service::work(_io_context))
	, _worker_thread(std::thread([&]{_io_context.run();}))
{
	Protocol::_logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("PROTOCOL"));
	_logger.add_attribute("Tag", boost::log::attributes::constant<std::string>("LSP"));
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
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::warning) << "did not read a valid message.";
		}
	}
	_is_done = true;
	BOOST_LOG(_logger) << "FINISHED";
	_work.reset();
	_worker_thread.join();
	return 0;
}

void LSP_server::on_exit(Params_exit& params)
{
	_is_done = true;
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_initialize(Params_initialize& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Document json_document;
	auto &allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kObjectType);
	result.AddMember("capabilities", _capabilities.get_json(allocator), allocator);
	reply(result);
}

void LSP_server::on_shutdown(Params_shutdown& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value result(rapidjson::kNullType);
	reply(result);
}

void LSP_server::on_textDocument_codeAction(Params_textDocument_codeAction& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_codeLens(Params_textDocument_codeLens& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value result(rapidjson::kNullType);
	reply(result);
}

void LSP_server::on_codeLens_resolve(Params_codeLens_resolve& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value result(rapidjson::kNullType);
	reply(result);
}

void LSP_server::on_textDocument_completion(Params_textDocument_completion& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_definition(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_didChange(Params_textDocument_didChange& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	auto& path = params._text_document._uri._path;
	auto file = _files.find(path);
	if (file != _files.end())
	{
		file->second.change_source_code(params._content_changes);
	}
}

void LSP_server::on_textDocument_didClose(Params_textDocument_didClose& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_didOpen(Params_textDocument_didOpen& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	auto& path = params._text_document._uri._path;
	auto& text = params._text_document._text;
	BOOST_LOG(_logger) << "create new P4_file \"" << path << "\"";
	if (auto command = find_command_for_path(path))
	{
		_files.emplace(std::piecewise_construct, std::forward_as_tuple(path), std::forward_as_tuple(*command, path, text));
	}
}

void LSP_server::on_textDocument_didSave(Params_textDocument_didSave& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_documentHighlight(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__
					   << "(document: \"" << params._text_document._uri._path
					   << "\", line: " << params._position._line
					   << ", character: " << params._position._character << ")";
	auto &path = params._text_document._uri._path;
	Location location;
	location._uri = path;
	location._range._start = params._position;
	location._range._end = params._position;
	auto file = _files.find(path);
	if (file != _files.end())
	{
		if (auto highlights = file->second.get_highlights(location))
		{
			rapidjson::Document json_document;
			auto& allocator = json_document.GetAllocator();
			rapidjson::Value result(rapidjson::kArrayType);
			for (auto& it : *highlights)
			{
				result.PushBack(it.get_json(allocator), allocator);
			}
			reply(result);
			return;
		}
	}
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_documentSymbol(Params_textDocument_documentSymbol& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Document json_document;
	auto& allocator = json_document.GetAllocator();
	rapidjson::Value result(rapidjson::kArrayType);
	auto& path = params._text_document._uri._path;
	auto file = _files.find(path);
	if (file != _files.end())
	{
		for (auto& it : file->second.get_symbols())
		{
			if (it._location._uri == path)
			{
				result.PushBack(it.get_json(allocator).Move(), allocator);
			}
		}
	}
	reply(result);
}

void LSP_server::on_textDocument_formatting(Params_textDocument_formatting& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_hover(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__
					   << "(document: \"" << params._text_document._uri._path
					   << "\", line: " << params._position._line
					   << ", character: " << params._position._character << ")";
	auto &path = params._text_document._uri._path;
	Location location;
	location._uri = path;
	location._range._start = params._position;
	location._range._end = params._position;
	auto file = _files.find(path);
	if (file != _files.end())
	{
		if (auto hover_content = file->second.get_hover(location))
		{
			BOOST_LOG(_logger) << "found hover content\n\"" << *hover_content << "\"";
			std::ostringstream os;
			os << "```p4\n" << *hover_content << "\n```";
#if 0
			// FIXME remove when settled on formatting the hover content
			os << "<div style=\"font-family: .SF NS Text; font-size: 10pt;\">\n<pre><code>"
			   << *hover_content << "\n</code></pre>\n</div>";
#endif
			Markup_content contents{MARKUP_KIND::markdown, os.str()};
			rapidjson::Document json_document;
			auto &allocator = json_document.GetAllocator();
			rapidjson::Value result(rapidjson::kObjectType);
			result.AddMember("contents", contents.get_json(allocator), allocator);
			reply(result);
			return;
		}
	}
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_implementation(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_rename(Params_textDocument_rename& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_signatureHelp(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_textDocument_typeDefinition(Params_text_document_position& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
	rapidjson::Value null(rapidjson::kNullType);
	reply(null);
}

void LSP_server::on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

void LSP_server::on_workspace_executeCommand(Params_workspace_executeCommand& params)
{
	BOOST_LOG(_logger) << __PRETTY_FUNCTION__;
}

boost::optional<std::string> LSP_server::read_message()
{
	BOOST_LOG(_logger) << "reading a new message";
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
		BOOST_LOG(_logger) << "got a line \"" << line << "\"";
		if (!_input_stream.good() && errno == EINTR)
		{
			BOOST_LOG(_logger) << "input error, clearing the stream";
			_input_stream.clear();
			continue;
		}
		if (0 == line.find_first_of('#'))
		{
			BOOST_LOG(_logger) << "skip a comment";
		}
		if (std::regex_match(line, match, content_length_regex))
		{
			content_length = std::stoull(match[1].str());
			BOOST_LOG(_logger) << "content length " << content_length;
		}
		else if (!line.empty())
		{
			BOOST_LOG(_logger) << "ignore another header line";
		}
		else
		{
			BOOST_LOG(_logger) << "an empty line, finished reading a message header";
			break;
		}
	}
	// discard unrealistically large requests
	if (content_length > 1 << 30)
	{
		BOOST_LOG(_logger) << "message is too big, size " << content_length;
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
			BOOST_LOG(_logger) << "got " << _input_stream.gcount() << " bytes, expected " << content_length << " content <" << content << ">";
			return boost::none;
		}
		BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::info) << "<--\n" << "Content-Length: " << content_length << "\r\n\r\n" << content;
		return std::move(content);
	}
	return boost::none;
}

boost::optional<std::string> LSP_server::find_command_for_path(const std::string& file)
{
	boost::optional<std::string> result;
	auto search = _commands.find(file);
	if (search != _commands.end())
	{
		result.emplace(search->second);
		return result;
	}
	for (boost::filesystem::path path(file); path.has_parent_path();)
	{
		path = path.parent_path();
		auto compile_commands_path = path / "compile_commands.json";
		if (boost::filesystem::exists(compile_commands_path))
		{
			boost::filesystem::ifstream ifs(compile_commands_path);
			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document json;
			json.ParseStream(isw);
			if (json.HasParseError() || !json.IsArray())
			{
				BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::error)
					<< "JSON parse error: " << rapidjson::GetParseError_En(json.GetParseError())
					<< " (" << json.GetErrorOffset() << ")";
				return result;
			}
			boost::char_separator<char> separator(" ");
			boost::tokenizer<boost::char_separator<char>> tokens(std::string(json.GetArray()[0]["command"].GetString()), separator);
			boost::filesystem::path compiler(*tokens.begin());
			std::string std_include;
			if (compiler.has_parent_path() && compiler.parent_path().has_parent_path())
			{
				auto std_include_path = compiler.parent_path().parent_path() / "share" / "p4c" / "p4include";
				if (boost::filesystem::exists(std_include_path))
				{
					std_include = std_include_path.c_str();
				}
				else
				{
					std_include_path = compiler.parent_path().parent_path() / "p4include";
					if (boost::filesystem::exists(std_include_path))
					{
						std_include = std_include_path.c_str();
					}
				}
			}
			for (auto& it : json.GetArray())
			{
				std::string the_file(it["file"].GetString());
				std::string command(it["command"].GetString());
				boost::char_separator<char> separator(" ");
				boost::tokenizer<boost::char_separator<char>> tokens(command, separator);
				std::string the_command("p4lsd");
				if (!std_include.empty())
				{
					the_command += " -I ";
					the_command += std_include;
				}
				auto token = tokens.begin();
				for (++token; token != tokens.end(); ++token)
				{
					the_command += " " + *token;
				}
				_commands.emplace(the_file, the_command);
			}
			search = _commands.find(file);
			if (search != _commands.end())
			{
				result.emplace(search->second);
				return result;
			}
			BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::warning)
				<< "did not find a matching command in \"" << compile_commands_path << "\"";
			return result;
		}
	}
	BOOST_LOG_SEV(_logger, boost::log::sinks::syslog::warning) << "did not find a compile_commands.json";
	return result;
}
