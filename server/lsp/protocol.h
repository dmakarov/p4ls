/*
 * -*- c++ -*-
 */

#pragma once

#include <info.h>

#include <rapidjson/document.h>

#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <string>
#include <vector>

enum class ERROR_CODES {
	// Defined by JSON RPC
	ParseError = -32700,
	InvalidRequest = -32600,
	MethodNotFound = -32601,
	InvalidParams = -32602,
	InternalError = -32603,
	serverErrorStart = -32099,
	serverErrorEnd = -32000,
	ServerNotInitialized = -32002,
	UnknownErrorCode = -32001,

	// Defined by the protocol.
	RequestCancelled = -32800
};

enum class COMPLETION_ITEM_KIND {
	Text = 1,
	Method = 2,
	Function = 3,
	Constructor = 4,
	Field = 5,
	Variable = 6,
	Class = 7,
	Interface = 8,
	Module = 9,
	Property = 10,
	Unit = 11,
	Value = 12,
	Enum = 13,
	Keyword = 14,
	Snippet = 15,
	Color = 16,
	File = 17,
	Reference = 18,
	Folder = 19,
	EnumMember = 20,
	Constant = 21,
	Struct = 22,
	Event = 23,
	Operator = 24,
	TypeParameter = 25
};

enum class SYMBOL_KIND {
	File = 1,
	Module = 2,
	Namespace = 3,
	Package = 4,
	Class = 5,
	Method = 6,
	Property = 7,
	Field = 8,
	Constructor = 9,
	Enum = 10,
	Interface = 11,
	Function = 12,
	Variable = 13,
	Constant = 14,
	String = 15,
	Number = 16,
	Boolean = 17,
	Array = 18,
	Object = 19,
	Key = 20,
	Null = 21,
	EnumMember = 22,
	Struct = 23,
	Event = 24,
	Operator = 25,
	TypeParameter = 26
};

enum class DOCUMENT_HIGHLIGHT_KIND { Text = 1, Read = 2, Write = 3 };

enum class MARKUP_KIND { plaintext, markdown };

enum class Trace_level { OFF = 0, MESSAGES = 1, VERBOSE = 2 };

enum class TEXT_DOCUMENT_SYNC_KIND {
	None = 0, /// documents should not be synced at all
	Full = 1, /// documents are synced by always sending the full content of the document
	Incremental = 2 /// documents are synced by sending the full content on open; after that only incremental updates to the document are send
};

namespace
{

COMPLETION_ITEM_KIND convert_int_to_completion_item_kind(const int v)
{
	switch (v)
	{
	case  1: return COMPLETION_ITEM_KIND::Text;
	case  2: return COMPLETION_ITEM_KIND::Method;
	case  3: return COMPLETION_ITEM_KIND::Function;
	case  4: return COMPLETION_ITEM_KIND::Constructor;
	case  5: return COMPLETION_ITEM_KIND::Field;
	case  6: return COMPLETION_ITEM_KIND::Variable;
	case  7: return COMPLETION_ITEM_KIND::Class;
	case  8: return COMPLETION_ITEM_KIND::Interface;
	case  9: return COMPLETION_ITEM_KIND::Module;
	case 10: return COMPLETION_ITEM_KIND::Property;
	case 11: return COMPLETION_ITEM_KIND::Unit;
	case 12: return COMPLETION_ITEM_KIND::Value;
	case 13: return COMPLETION_ITEM_KIND::Enum;
	case 14: return COMPLETION_ITEM_KIND::Keyword;
	case 15: return COMPLETION_ITEM_KIND::Snippet;
	case 16: return COMPLETION_ITEM_KIND::Color;
	case 17: return COMPLETION_ITEM_KIND::File;
	case 18: return COMPLETION_ITEM_KIND::Reference;
	case 19: return COMPLETION_ITEM_KIND::Folder;
	case 20: return COMPLETION_ITEM_KIND::EnumMember;
	case 21: return COMPLETION_ITEM_KIND::Constant;
	case 22: return COMPLETION_ITEM_KIND::Struct;
	case 23: return COMPLETION_ITEM_KIND::Event;
	case 24: return COMPLETION_ITEM_KIND::Operator;
	case 25: return COMPLETION_ITEM_KIND::TypeParameter;
	}
	BOOST_LOG_TRIVIAL(error) << "Unknown completion item kind " << v;
	return COMPLETION_ITEM_KIND::Text;
}

SYMBOL_KIND convert_int_to_symbol_kind(const int v)
{
	switch (v)
	{
	case  1: return SYMBOL_KIND::File;
	case  2: return SYMBOL_KIND::Module;
	case  3: return SYMBOL_KIND::Namespace;
	case  4: return SYMBOL_KIND::Package;
	case  5: return SYMBOL_KIND::Class;
	case  6: return SYMBOL_KIND::Method;
	case  7: return SYMBOL_KIND::Property;
	case  8: return SYMBOL_KIND::Field;
	case  9: return SYMBOL_KIND::Constructor;
	case 10: return SYMBOL_KIND::Enum;
	case 11: return SYMBOL_KIND::Interface;
	case 12: return SYMBOL_KIND::Function;
	case 13: return SYMBOL_KIND::Variable;
	case 14: return SYMBOL_KIND::Constant;
	case 15: return SYMBOL_KIND::String;
	case 16: return SYMBOL_KIND::Number;
	case 17: return SYMBOL_KIND::Boolean;
	case 18: return SYMBOL_KIND::Array;
	case 19: return SYMBOL_KIND::Object;
	case 20: return SYMBOL_KIND::Key;
	case 21: return SYMBOL_KIND::Null;
	case 22: return SYMBOL_KIND::EnumMember;
	case 23: return SYMBOL_KIND::Struct;
	case 24: return SYMBOL_KIND::Event;
	case 25: return SYMBOL_KIND::Operator;
	case 26: return SYMBOL_KIND::TypeParameter;
	}
	BOOST_LOG_TRIVIAL(error) << "Unknown symbol kind " << v;
	return SYMBOL_KIND::File;
}

MARKUP_KIND convert_string_to_markup_kind(const std::string& v)
{
	if (v == "plaintext") return MARKUP_KIND::plaintext;
	if (v == "markdown") return MARKUP_KIND::markdown;
	BOOST_LOG_TRIVIAL(error) << "Unknown markup kind " << v;
	return MARKUP_KIND::plaintext;
}

const char* convert_markup_kind_to_string(const MARKUP_KIND kind)
{
	switch (kind)
	{
	case MARKUP_KIND::plaintext: return "plaintext";
	case MARKUP_KIND::markdown: return "markdown";
	default:;
		BOOST_LOG_TRIVIAL(error) << "Unknown markup kind " << static_cast<int>(kind);
	}
	return "plaintext";
}

} // namespace

struct URI {
	URI() = default;
	explicit URI(const std::string& path) : _path(path)
	{}

	void set_from_path(const std::string& path)
	{
		_path = std::move(path);
	}

	void set_from_uri(const std::string& uri)
	{
		auto pos = uri.find("://");
		if (pos == std::string::npos)
		{
			_path = std::move(uri);
		}
		else
		{
			_path = uri.substr(pos + 3);
		}
	}

	bool operator==(const URI& other) const
	{
		return _path == other._path;
	}

	std::string _path;
};

std::ostream& operator<<(std::ostream& os, const URI& item);

struct Workspace_folder {
	Workspace_folder(const rapidjson::Value&)
	{
	}
	std::string _uri;
	std::string _name;
};

struct Text_document_client_capabilities {

	explicit Text_document_client_capabilities(const rapidjson::Value& json)
	{
		if (json.HasMember("synchronization") && !json["synchronization"].IsNull())
		{
			_synchronization.emplace(Synchronization(json["synchronization"]));
		}
		if (json.HasMember("completion") && !json["completion"].IsNull())
		{
			_completion.emplace(Completion(json["completion"]));
		}
		if (json.HasMember("hover") && !json["hover"].IsNull())
		{
			_hover.emplace(Hover(json["hover"]));
		}
		if (json.HasMember("signatureHelp") && !json["signatureHelp"].IsNull())
		{
			_signature_help.emplace(Signature_help(json["signatureHelp"]));
		}
		if (json.HasMember("references") && !json["references"].IsNull())
		{
			_references.emplace(References(json["references"]));
		}
		if (json.HasMember("documentHighlight") && !json["documentHighlight"].IsNull())
		{
			_document_highlight.emplace(Document_highlight(json["documentHighlight"]));
		}
		if (json.HasMember("documentSymbol") && !json["documentSymbol"].IsNull())
		{
			_document_symbol.emplace(Document_symbol(json["documentSymbol"]));
		}
		if (json.HasMember("formatting") && !json["formatting"].IsNull())
		{
			_formatting.emplace(Formatting(json["formatting"]));
		}
		if (json.HasMember("rangeFormatting") && !json["rangeFormatting"].IsNull())
		{
			_range_formatting.emplace(Range_formatting(json["rangeFormatting"]));
		}
		if (json.HasMember("onTypeFormatting") && !json["onTypeFormatting"].IsNull())
		{
			_on_type_formatting.emplace(On_type_formatting(json["onTypeFormatting"]));
		}
		if (json.HasMember("definition") && !json["definition"].IsNull())
		{
			_definition.emplace(Definition(json["definition"]));
		}
		if (json.HasMember("typeDefinition") && !json["typeDefinition"].IsNull())
		{
			_type_definition.emplace(Type_definition(json["typeDefinition"]));
		}
		if (json.HasMember("implementation") && !json["implementation"].IsNull())
		{
			_implementation.emplace(Implementation(json["implementation"]));
		}
		if (json.HasMember("codeAction") && !json["codeAction"].IsNull())
		{
			_code_action.emplace(Code_action(json["codeAction"]));
		}
		if (json.HasMember("codeLens") && !json["codeLens"].IsNull())
		{
			_code_lens.emplace(Code_lens(json["codeLens"]));
		}
		if (json.HasMember("documentLink") && !json["documentLink"].IsNull())
		{
			_document_link.emplace(Document_link(json["documentLink"]));
		}
		if (json.HasMember("colorProvider") && !json["colorProvider"].IsNull())
		{
			_color_provider.emplace(Color_provider(json["colorProvider"]));
		}
		if (json.HasMember("rename") && !json["rename"].IsNull())
		{
			_rename.emplace(Rename(json["rename"]));
		}
		if (json.HasMember("publishDiagnostics") && !json["publishDiagnostics"].IsNull())
		{
			_publish_diagnostics.emplace(Publish_diagnostics(json["publishDiagnostics"]));
		}
	}

	struct Synchronization {
		explicit Synchronization(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("willSave") && !json["willSave"].IsNull())
			{
				_will_save.emplace(json["willSave"].GetBool());
			}
			if (json.HasMember("willSaveWaitUntil") && !json["willSaveWaitUntil"].IsNull())
			{
				_will_save_wait_until.emplace(json["willSaveWaitUntil"].GetBool());
			}
			if (json.HasMember("didSave") && !json["didSave"].IsNull())
			{
				_did_save.emplace(json["didSave"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether text document synchronization supports dynamic registration.
		boost::optional<bool> _will_save;            /// The client supports sending will save notifications.
		boost::optional<bool> _will_save_wait_until; /// The client supports sending a will save request and waits for a response providing text edits which will be applied to the document before it is saved.
		boost::optional<bool> _did_save;             /// The client supports did save notifications.
	};

	struct Completion {
		explicit Completion(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("contextSupport") && !json["contextSupport"].IsNull())
			{
				_context_support.emplace(json["contextSupport"].GetBool());
			}
			if (json.HasMember("completionItem") && !json["completionItem"].IsNull())
			{
				_completion_item.emplace(Completion_item(json["completionItem"]));
			}
			if (json.HasMember("completionItemKind") && !json["completionItemKind"].IsNull())
			{
				_completion_item_kind.emplace(Completion_item_kind(json["completionItemKind"]));
			}
		}

		struct Completion_item {
			explicit Completion_item(const rapidjson::Value& json)
			{
				if (json.HasMember("snippetSupport") && !json["snippetSupport"].IsNull())
				{
					_snippet_support.emplace(json["snippetSupport"].GetBool());
				}
				if (json.HasMember("commitCharactersSupport") && !json["commitCharactersSupport"].IsNull())
				{
					_commit_characters_support.emplace(json["commitCharactersSupport"].GetBool());
				}
				if (json.HasMember("documentationFormat") && !json["documentationFormat"].IsNull())
				{
					std::vector<MARKUP_KIND> values;
					for (auto *it = json["documentationFormat"].Begin(); it != json["documentationFormat"].End(); ++it)
					{
						values.emplace_back(convert_string_to_markup_kind(it->GetString()));
					}
					_documentation_format.emplace(values);
				}
			}

			boost::optional<bool> _snippet_support; /// Client supports snippets as insert text. A snippet can define tab stops and placeholders with `$1`, `$2` and `${3:foo}`. `$0` defines the final tab stop, it defaults to the end of the snippet. Placeholders with equal identifiers are linked, that is typing in one will update others too.
			boost::optional<bool> _commit_characters_support; /// Client supports commit characters on a completion item.
			boost::optional<std::vector<MARKUP_KIND>> _documentation_format; /// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
		};

		struct Completion_item_kind {
			explicit Completion_item_kind(const rapidjson::Value& json)
			{
				if (json.HasMember("valueSet") && !json["valueSet"].IsNull())
				{
					std::vector<COMPLETION_ITEM_KIND> values;
					for (auto *it = json["valueSet"].Begin(); it != json["valueSet"].End(); ++it)
					{
						values.emplace_back(convert_int_to_completion_item_kind(it->GetInt()));
					}
					_value_set.emplace(values);
				}
			}

			boost::optional<std::vector<COMPLETION_ITEM_KIND>> _value_set; /// The completion item kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the completion items kinds from `Text` to `Reference` as defined in the initial version of the protocol.
		};

		boost::optional<Completion_item> _completion_item; /// The client supports the following `CompletionItem` specific capabilities.
		boost::optional<Completion_item_kind> _completion_item_kind;
		boost::optional<bool> _dynamic_registration; /// Whether completion supports dynamic registration.
		boost::optional<bool> _context_support;		/// The client supports to send additional context information for a `textDocument/completion` request.
	};

	struct Hover {
		explicit Hover(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("contentFormat") && !json["contentFormat"].IsNull())
			{
				std::vector<MARKUP_KIND> values;
				for (auto *it = json["contentFormat"].Begin(); it != json["contentFormat"].End(); ++it)
				{
					values.emplace_back(convert_string_to_markup_kind(it->GetString()));
				}
				_content_format.emplace(values);
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether hover supports dynamic registration.
		boost::optional<std::vector<MARKUP_KIND>> _content_format; /// Client supports the follow content formats for the content property. The order describes the preferred format of the client.
	};

	struct Signature_help {
		explicit Signature_help(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("signatureInformation") && !json["signatureInformation"].IsNull())
			{
				_signature_information.emplace(Signature_information(json["signatureInformation"]));
			}
		}

		struct Signature_information {
			explicit Signature_information(const rapidjson::Value& json)
			{
				if (json.HasMember("documentationFormat") && !json["documentationFormat"].IsNull())
				{
					std::vector<MARKUP_KIND> values;
					for (auto *it = json["documentationFormat"].Begin(); it != json["documentationFormat"].End(); ++it)
					{
						values.emplace_back(convert_string_to_markup_kind(it->GetString()));
					}
					_documentation_format.emplace(values);
				}
			}

			boost::optional<std::vector<MARKUP_KIND>> _documentation_format; /// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
		};

		boost::optional<bool> _dynamic_registration; /// Whether signature help supports dynamic registration.
		boost::optional<Signature_information> _signature_information; /// The client supports the following `SignatureInformation` specific properties.
	};

	struct References {
		explicit References(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether references supports dynamic registration.
	};

	struct Document_highlight {
		explicit Document_highlight(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether document highlight supports dynamic registration.
	};

	struct Document_symbol {
		explicit Document_symbol(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("symbolKind") && !json["symbolKind"].IsNull())
			{
				_symbol_kind.emplace(Symbol_kind(json["symbolKind"]));
			}
		}

		struct Symbol_kind {
			explicit Symbol_kind(const rapidjson::Value& json)
			{
				if (json.HasMember("valueSet") && !json["valueSet"].IsNull())
				{
					std::vector<SYMBOL_KIND> values;
					for (auto *it = json["valueSet"].Begin(); it != json["valueSet"].End(); ++it)
					{
						values.emplace_back(convert_int_to_symbol_kind(it->GetInt()));
					}
					_value_set.emplace(values);
				}
			}

			boost::optional<std::vector<SYMBOL_KIND>> _value_set; /// The symbol kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the symbol kinds from `File` to `Array` as defined in the initial version of the protocol.
		};

		boost::optional<bool> _dynamic_registration; /// Whether document symbol supports dynamic registration.
		boost::optional<Symbol_kind> _symbol_kind; /// Specific capabilities for the `SymbolKind`.
	};

	struct Formatting {
		explicit Formatting(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether formatting supports dynamic registration.
	};

	struct Range_formatting {
		explicit Range_formatting(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether range formatting supports dynamic registration.
	};

	struct On_type_formatting {
		explicit On_type_formatting(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether on type formatting supports dynamic registration.
	};

	struct Definition {
		explicit Definition(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether definition supports dynamic registration.
	};

	struct Type_definition {
		explicit Type_definition(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether typeDefinition supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Implementation {
		explicit Implementation(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether implementation supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Code_action {
		explicit Code_action(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether code action supports dynamic registration.
	};

	struct Code_lens {
		explicit Code_lens(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether code lens supports dynamic registration.
	};

	struct Document_link {
		explicit Document_link(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether document link supports dynamic registration.
	};

	struct Color_provider {
		explicit Color_provider(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether colorProvider supports dynamic registration. If this is set to `true` the client supports the new `(ColorProviderOptions & TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Rename {
		explicit Rename(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}

		boost::optional<bool> _dynamic_registration; /// Whether rename supports dynamic registration.
	};

	struct Publish_diagnostics {
		explicit Publish_diagnostics(const rapidjson::Value& json)
		{
			if (json.HasMember("relatedInformation") && !json["relatedInformation"].IsNull())
			{
				_related_information.emplace(json["relatedInformation"].GetBool());
			}
		}

		boost::optional<bool> _related_information; /// Whether the clients accepts diagnostics with related information.
	};

	boost::optional<Synchronization> _synchronization;
	boost::optional<Completion> _completion; /// Capabilities specific to the `textDocument/completion`
	boost::optional<Hover> _hover; /// Capabilities specific to the `textDocument/hover`
	boost::optional<Signature_help> _signature_help; /// Capabilities specific to the `textDocument/signatureHelp`
	boost::optional<References> _references; /// Capabilities specific to the `textDocument/references`
	boost::optional<Document_highlight> _document_highlight; /// Capabilities specific to the `textDocument/documentHighlight`
	boost::optional<Document_symbol> _document_symbol; /// Capabilities specific to the `textDocument/documentSymbol`
	boost::optional<Formatting> _formatting; /// Capabilities specific to the `textDocument/formatting`
	boost::optional<Range_formatting> _range_formatting; /// Capabilities specific to the `textDocument/rangeFormatting`
	boost::optional<On_type_formatting> _on_type_formatting; /// Capabilities specific to the `textDocument/onTypeFormatting`
	boost::optional<Definition> _definition; /// Capabilities specific to the `textDocument/definition`
	boost::optional<Type_definition> _type_definition; /// Capabilities specific to the `textDocument/typeDefinition`
	boost::optional<Implementation> _implementation; /// Capabilities specific to the `textDocument/implementation`.
	boost::optional<Code_action> _code_action; /// Capabilities specific to the `textDocument/codeAction`
	boost::optional<Code_lens> _code_lens; /// Capabilities specific to the `textDocument/codeLens`
	boost::optional<Document_link> _document_link; /// Capabilities specific to the `textDocument/documentLink`
	boost::optional<Color_provider> _color_provider; /// Capabilities specific to the `textDocument/documentColor` and the `textDocument/colorPresentation` request.
	boost::optional<Rename> _rename; /// Capabilities specific to the `textDocument/rename`
	boost::optional<Publish_diagnostics> _publish_diagnostics; /// Capabilities specific to `textDocument/publishDiagnostics`.
};


struct Workspace_client_capabilities {

	explicit Workspace_client_capabilities(const rapidjson::Value& json)
	{
		if (json.HasMember("applyEdit") && !json["applyEdit"].IsNull())
		{
			_apply_edit.emplace(json["applyEdit"].GetBool());
		}
		if (json.HasMember("workspaceFolders") && !json["workspaceFolders"].IsNull())
		{
			_workspace_folders.emplace(json["workspaceFolders"].GetBool());
		}
		if (json.HasMember("configuration") && !json["configuration"].IsNull())
		{
			_configuration.emplace(json["configuration"].GetBool());
		}
		if (json.HasMember("workspaceEdit") && !json["workspaceEdit"].IsNull())
		{
			_workspace_edit.emplace(Workspace_edit(json["workspaceEdit"]));
		}
		if (json.HasMember("didChangeConfiguration") && !json["didChangeConfiguration"].IsNull())
		{
			_did_change_configuration.emplace(Did_change_configuration(json["didChangeConfiguration"]));
		}
		if (json.HasMember("didChangeWatchedFiles") && !json["didChangeWatchedFiles"].IsNull())
		{
			_did_change_watched_files.emplace(Did_change_watched_files(json["didChangeWatchedFiles"]));
		}
		if (json.HasMember("executeCommand") && !json["executeCommand"].IsNull())
		{
			_execute_command.emplace(Execute_command(json["executeCommand"]));
		}
		if (json.HasMember("symbol") && !json["symbol"].IsNull())
		{
			_symbol.emplace(Symbol(json["symbol"]));
		}
	}

	struct Workspace_edit {
		explicit Workspace_edit(const rapidjson::Value& json)
		{
			if (json.HasMember("documentChanges") && !json["documentChanges"].IsNull())
			{
				_document_changes.emplace(json["documentChanges"].GetBool());
			}
		}
		boost::optional<bool> _document_changes; /// The client supports versioned document changes in `WorkspaceEdit`s
	};

	struct Did_change_configuration {
		explicit Did_change_configuration(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> _dynamic_registration; /// Did change configuration notification supports dynamic registration.
	};

	struct Did_change_watched_files {
		explicit Did_change_watched_files(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> _dynamic_registration; /// Did change watched files notification supports dynamic registration.
	};

	struct Execute_command {
		explicit Execute_command(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> _dynamic_registration; /// execute command supports dynamic registration
	};

	struct Symbol {
		explicit Symbol(const rapidjson::Value& json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				_dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("symbolKind") && !json["symbolKind"].IsNull())
			{
				_symbol_kind.emplace(Symbol_kind(json["symbolKind"]));
			}
		}

		struct Symbol_kind {
			explicit Symbol_kind(const rapidjson::Value& json)
			{
				if (json.HasMember("valueSet") && !json["valueSet"].IsNull())
				{
					std::vector<SYMBOL_KIND> values;
					for (auto *it = json["valueSet"].Begin(); it != json["valueSet"].End(); ++it)
					{
						values.emplace_back(convert_int_to_symbol_kind(it->GetInt()));
					}
					_value_set.emplace(values);
				}
			}
			/**
			 * The symbol kind values the client supports. When this
			 * property exists the client also guarantees that it will
			 * handle values outside its set gracefully and falls back to
			 * a default value when unknown.
			 *
			 * If this property is not present the client only supports
			 * the symbol kinds from `File` to `Array` as defined in the
			 * initial version of the protocol.
			 */
			boost::optional<std::vector<SYMBOL_KIND>> _value_set;
		};

		boost::optional<bool> _dynamic_registration; /// Symbol request supports dynamic registration.
		boost::optional<Symbol_kind> _symbol_kind;   /// Specific capabilities for the `SymbolKind` in the `workspace/symbol` request
	};

	boost::optional<Workspace_edit> _workspace_edit;                     /// capabilities specific to `WorkspaceEdit`s
	boost::optional<Did_change_configuration> _did_change_configuration; /// capabilities specific to the `workspace/didChangeConfiguration` notification
	boost::optional<Did_change_watched_files> _did_change_watched_files; /// capabilities specific to the `workspace/didChangeWatchedFiles` notification
	boost::optional<Execute_command> _execute_command;                   /// capabilities specific to the `workspace/executeCommand` request
	boost::optional<Symbol> _symbol;                                     /// capabilities specific to the `workspace/symbol` request
	boost::optional<bool> _apply_edit;        /// support applying batch edits to the workspace
	boost::optional<bool> _workspace_folders; /// support workspace folders
	boost::optional<bool> _configuration;     /// support `workspace/configuration` requests
};


struct Client_capabilities {

	void set(const rapidjson::Value& json)
	{
		if (json.HasMember("workspace") && !json["workspace"].IsNull())
		{
			_workspace.emplace(Workspace_client_capabilities{std::move(json["workspace"])});
		}
		if (json.HasMember("textDocument") && !json["textDocument"].IsNull())
		{
			_text_document.emplace(Text_document_client_capabilities{std::move(json["textDocument"])});
		}
	}

	boost::optional<Workspace_client_capabilities> _workspace;
	boost::optional<Text_document_client_capabilities> _text_document;
};


struct Server_capabilities {

	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator);

	struct Text_document_sync_options {

		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator);

		struct Save_options {
			rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
			{
				rapidjson::Value result(rapidjson::kObjectType);
				if (_include_text)
				{
					result.AddMember("includeText", *_include_text, allocator);
				}
				return result;
			}

			boost::optional<bool> _include_text; /// the client is supposed to include the content on save
		};

		boost::optional<bool> _open_close;           /// open and close notifications are sent to the server
		boost::optional<bool> _will_save;            /// will save notifications are sent to the server
		boost::optional<bool> _will_save_wait_until; /// will save wait until requests are sent to the server
		boost::optional<Save_options> _save;         /// save notifications are sent to the server
		boost::optional<TEXT_DOCUMENT_SYNC_KIND> _change; /// change notifications are sent to the server
	};

	struct Completion_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (_resolve_provider)
			{
				result.AddMember("resolveProvider", *_resolve_provider, allocator);
			}
			if (_trigger_characters)
			{
				result.AddMember("triggerCharacters", rapidjson::StringRef(_trigger_characters->c_str()), allocator);
			}
			return result;
		}

		boost::optional<bool> _resolve_provider;          /// the server provides support to resolve additional information for a completion item
		boost::optional<std::string> _trigger_characters; /// the characters that trigger completion automatically.
	};

	struct Signature_help_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (_trigger_characters)
			{
				result.AddMember("triggerCharacters", rapidjson::StringRef(_trigger_characters->c_str()), allocator);
			}
			return result;
		}

		boost::optional<std::string> _trigger_characters; /// the characters that trigger signature help automatically
	};

	struct Code_lens_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (_resolve_provider)
			{
				result.AddMember("resolveProvider", *_resolve_provider, allocator);
			}
			return result;
		}

		boost::optional<bool> _resolve_provider; /// code lens has a resolve provider as well
	};

	struct Document_on_type_formatting_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			result.AddMember("firstTriggerCharacter", rapidjson::StringRef(_first_trigger_character.c_str()), allocator);
			if (_more_trigger_character)
			{
				result.AddMember("moreTriggerCharacter", rapidjson::StringRef(_more_trigger_character->c_str()), allocator);
			}
			return result;
		}

		std::string _first_trigger_character; /// A character on which formatting should be triggered, like `}`
		boost::optional<std::string> _more_trigger_character; /// More trigger characters.
	};

	struct Document_link_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (resolve_provider)
			{
				result.AddMember("resolveProvider", *resolve_provider, allocator);
			}
			return result;
		}

		boost::optional<bool> resolve_provider; /// Document links have a resolve provider as well.
	};

	struct Execute_command_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value a(rapidjson::kArrayType);
			for (auto& it : _commands)
			{
				a.PushBack(rapidjson::StringRef(it.c_str()), allocator);
			}
			rapidjson::Value result(rapidjson::kObjectType);
			result.AddMember("commands", a, allocator);
			return result;
		}

		std::vector<std::string> _commands; ///  The commands to be executed on the server
	};

	struct Workspace_folders {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (_supported)
			{
				result.AddMember("supported", *_supported, allocator);
			}
			if (_change_notifications)
			{
				result.AddMember("changeNotifications", rapidjson::StringRef(_change_notifications->c_str()), allocator);
			}
			return result;
		}

		boost::optional<bool> _supported; /// The server has support for workspace folders
		boost::optional<std::string> _change_notifications; /// Whether the server wants to receive workspace folder change notifications. If a strings is provided the string is treated as a ID under which the notification is registed on the client side. The ID can be used to unregister for these events using the `client/unregisterCapability` request.
	};

	struct Workspace {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			if (_workspace_folders)
			{
				result.AddMember("workspaceFolders", _workspace_folders->get_json(allocator), allocator);
			}
			return result;
		}

		boost::optional<Workspace_folders> _workspace_folders; /// The server supports workspace folder
	};

	struct Color_provider_options {
		rapidjson::Value get_json(rapidjson::Document::AllocatorType&)
		{
			rapidjson::Value result(rapidjson::kObjectType);
			return result;
		}
	};

	boost::optional<Workspace> _workspace; /// Workspace specific server capabilities
	boost::optional<Text_document_sync_options> _text_document_sync; /// Defines how text documents are synced. Is either a detailed structure defining each notification or for backwards compatibility the TextDocumentSyncKind number. If omitted it defaults to `TextDocumentSyncKind.None`.
	boost::optional<Completion_options> _completion_provider; /// The server provides completion support.
	boost::optional<Signature_help_options> _signature_help_provider; /// The server provides signature help support
	boost::optional<Code_lens_options> _code_lens_provider; /// The server provides code lens.
	boost::optional<Document_on_type_formatting_options> _document_on_type_formatting_provider; /// The server provides document formatting on typing.
	boost::optional<Document_link_options> _document_link_provider; /// The server provides document link support.
	boost::optional<Color_provider_options> _color_provider; /// The server provides color provider support.
	boost::optional<Execute_command_options> _execute_command_provider; /// The server provides execute command support.
	boost::optional<bool> _hover_provider; /// The server provides hover support.
	boost::optional<bool> _definition_provider; /// The server provides goto definition support
	boost::optional<bool> _type_definition_provider; /// The server provides Goto Type Definition support
	boost::optional<bool> _implementation_provider; /// The server provides Goto Implementation support
	boost::optional<bool> _references_provider; /// The server provides find references support
	boost::optional<bool> _document_highlight_provider; /// The server provides document highlight support
	boost::optional<bool> _document_symbol_provider; /// The server provides document symbol support.
	boost::optional<bool> _workspace_symbol_provider; /// The server provides workspace symbol support.
	boost::optional<bool> _code_action_provider; /// The server provides code actions.
	boost::optional<bool> _document_formatting_provider; /// The server provides document formatting.
	boost::optional<bool> _document_range_formatting_provider; /// The server provides document range formatting.
	boost::optional<bool> _rename_provider; /// The server provides rename support.
};


struct Range {
	Range() = default;

	Range(const Info& info)
	{
		set(info);
	}

	Range(const rapidjson::Value& json) : _start(json["start"]), _end(json["end"])
	{}

	void set(const Info& info)
	{
		_start._line = info.get_start().get_line_number() - 1;
		_start._character = info.get_start().get_column_number();
		_end._line = info.get_end().get_line_number() - 1;
		_end._character = info.get_end().get_column_number();
	}

	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("start", _start.get_json(allocator), allocator);
		result.AddMember("end", _end.get_json(allocator), allocator);
		return result;
	}

	Position _start;	// the range's start position
	Position _end;		// the range's end position
};

std::ostream& operator<<(std::ostream& os, const Range& range);
bool operator<(const Range& lhs, const Range& rhs);
bool operator&(const Range& lhs, const Range& rhs);


/**
 * Represents a location inside a resource, such as a line inside a
 * text file.
 */
struct Location {
	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("uri", rapidjson::StringRef(_uri.c_str()), allocator);
		result.AddMember("range", _range.get_json(allocator), allocator);
		return result;
	}

	std::string _uri;
	Range _range;
};

std::ostream& operator<<(std::ostream& os, const Location& location);
bool operator==(const Location& lhs, const Location& rhs);
bool operator<(const Location& lhs, const Location& rhs);


/**
 * Represents information about programming constructs like variables,
 * classes, interfaces etc.
 */
struct Symbol_information {
	Symbol_information(std::string name, SYMBOL_KIND kind, const Location& location, const boost::optional<std::string>& container)
		: _name(std::move(name))
		, _kind(kind)
		, _deprecated(false)
		, _location(std::move(location))
		, _container_name(std::move(container))
	{}

	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("name", rapidjson::StringRef(_name.c_str()), allocator);
		result.AddMember("kind", static_cast<int>(_kind), allocator);
		result.AddMember("deprecated", _deprecated, allocator);
		result.AddMember("location", _location.get_json(allocator), allocator);
		if (_container_name)
		{
			result.AddMember("containerName", rapidjson::StringRef(_container_name->c_str()), allocator);
		}
		return result;
	}

	std::string _name;	// the name of this symbol
	SYMBOL_KIND _kind;	// the kind of this symbol
	bool _deprecated;	// indicates if this symbol is deprecated
	/**
	 * The location of this symbol. The location's range is used by a tool
	 * to reveal the location in the editor. If the symbol is selected in the
	 * tool the range's start information is used to position the cursor. So
	 * the range usually spans more then the actual symbol's name and does
	 * normally include things like visibility modifiers.
	 *
	 * The range doesn't have to denote a node range in the sense of a abstract
	 * syntax tree. It can therefore not be used to re-construct a hierarchy of
	 * the symbols.
	 */
	Location _location;
	/**
	 * The name of the symbol containing this symbol. This information is for
	 * user interface purposes (e.g. to render a qualifier in the user interface
	 * if necessary). It can't be used to re-infer a hierarchy for the document
	 * symbols.
	 */
	boost::optional<std::string> _container_name;
};


struct Text_document_item {

	bool set(const rapidjson::Value& json)
	{
		if (!json.HasMember("uri") || !json.HasMember("languageId") || !json.HasMember("version") || !json.HasMember("text"))
		{
			return false;
		}
		_uri.set_from_uri(json["uri"].GetString());
		_language_id = json["languageId"].GetString();
		_version = json["version"].GetInt();
		_text = json["text"].GetString();
		return true;
	}

	URI _uri;                 /// text document's URI
	std::string _language_id; /// text document's language identifier
	int _version = 0;         /// version number of this document (it will increase after each change, including undo/redo)
	std::string _text;        /// content of the opened text document
};


struct Text_document_identifier {

	virtual bool set(const rapidjson::Value& json)
	{
		auto it = json.FindMember("uri");
		if (it == json.MemberEnd())
		{
			return false;
		}
		_uri.set_from_uri(it->value.GetString());
		return true;
	}

	URI _uri;                 /// text document's URI
};


struct Versioned_text_document_identifier : public Text_document_identifier {

	bool set(const rapidjson::Value& json) override
	{
		if (!Text_document_identifier::set(json))
		{
			return false;
		}
		auto it = json.FindMember("version");
		if (it != json.MemberEnd())
		{
			_version.emplace(it->value.GetInt());
		}
		return true;
	}

	boost::optional<int> _version;
};


struct Text_document_content_change_event {

	bool set(const rapidjson::Value& json)
	{
		auto it = json.FindMember("text");
		if (it == json.MemberEnd())
		{
			return false;
		}
		_text = it->value.GetString();
		it = json.FindMember("range");
		if (it != json.MemberEnd())
		{
			_range.emplace(it->value);
		}
		it = json.FindMember("rangeLength");
		if (it != json.MemberEnd())
		{
			_range_length.emplace(it->value.GetInt());
		}
		return true;
	}

	boost::optional<Range> _range;                /// the range of the document that changed
	boost::optional<unsigned int> _range_length;  /// the length of the range that got replaced
	std::string _text;                            /// the new text of the range/document
};


struct Markup_content {
	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("kind", rapidjson::StringRef(convert_markup_kind_to_string(_kind)), allocator);
		result.AddMember("value", rapidjson::StringRef(_value.c_str()), allocator);
		return result;
	}

	MARKUP_KIND _kind;
	std::string _value;
};


struct Text_document_highlight {
	Text_document_highlight(const Range& range, DOCUMENT_HIGHLIGHT_KIND kind)
		: _range(std::move(range))
		, _kind(kind)
	{}

	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("range", _range.get_json(allocator), allocator);
		result.AddMember("kind", static_cast<int>(_kind), allocator);
		return result;
	}

	Range _range;
	DOCUMENT_HIGHLIGHT_KIND _kind;
};


struct Params_exit {
};

bool set_params_from_json(const rapidjson::Value& json, Params_exit& params);

struct Params_initialize {
	int _process_id;
	URI _root_uri;
	Client_capabilities _capabilities;
	boost::optional<Trace_level> _trace;
	boost::optional<std::vector<std::string>> _initialization_options;
	boost::optional<std::vector<Workspace_folder>> _workspace_folders;
};

bool set_params_from_json(const rapidjson::Value& json, Params_initialize& params);

struct Params_shutdown {
};

bool set_params_from_json(const rapidjson::Value& json, Params_shutdown& params);

struct Params_text_document_position {

	bool set(const rapidjson::Value& json)
	{
		if (json.HasMember("textDocument") && json.HasMember("position"))
		{
			return _text_document.set(json["textDocument"]) && _position.set(json["position"]);
		}
		return false;
	}

	Text_document_identifier _text_document;
	Position _position;
};

bool set_params_from_json(const rapidjson::Value& json, Params_text_document_position& params);

struct Params_textDocument_codeAction {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_codeAction& params);

struct Params_textDocument_codeLens {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_codeLens& params);

struct Params_codeLens_resolve {
};

bool set_params_from_json(const rapidjson::Value& json, Params_codeLens_resolve& params);

struct Params_textDocument_completion {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_completion& params);

struct Params_textDocument_didChange {

	bool set(const rapidjson::Value& json)
	{
		auto it = json.FindMember("textDocument");
		if (it == json.MemberEnd())
		{
			return false;
		}
		if (!_text_document.set(it->value))
		{
			return false;
		}
		it = json.FindMember("contentChanges");
		if (it == json.MemberEnd())
		{
			return false;
		}
		for (auto& e : it->value.GetArray())
		{
			Text_document_content_change_event event;
			if (!event.set(e))
			{
				return false;
			}
			_content_changes.emplace_back(event);
		}
		return true;
	}

	Versioned_text_document_identifier _text_document;
	std::vector<Text_document_content_change_event> _content_changes;
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didChange& params);

struct Params_textDocument_didClose {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didClose& params);

struct Params_textDocument_didOpen {
	Text_document_item _text_document;
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didOpen& params);

struct Params_textDocument_didSave {
	Text_document_identifier _text_document;
	boost::optional<std::string> _text;
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_didSave& params);

struct Params_textDocument_documentSymbol {
	Text_document_identifier _text_document;
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_documentSymbol& params);

struct Params_textDocument_formatting {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_formatting& params);

struct Params_textDocument_onTypeFormatting {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_onTypeFormatting& params);

struct Params_textDocument_rangeFormatting {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_rangeFormatting& params);

struct Params_textDocument_rename {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_rename& params);

struct Params_textDocument_switchSourceHeader {
};

bool set_params_from_json(const rapidjson::Value& json, Params_textDocument_switchSourceHeader& params);

struct Params_workspace_didChangeConfiguration {
};

bool set_params_from_json(const rapidjson::Value& json, Params_workspace_didChangeConfiguration& params);

struct Params_workspace_didChangeWatchedFiles {
};

bool set_params_from_json(const rapidjson::Value& json, Params_workspace_didChangeWatchedFiles& params);

struct Params_workspace_executeCommand {
};

bool set_params_from_json(const rapidjson::Value& json, Params_workspace_executeCommand& params);

class Protocol {
public:
	static boost::log::sources::severity_logger<int> _logger;
	virtual ~Protocol() = default;

	virtual void on_exit(Params_exit& params) = 0;
	virtual void on_initialize(Params_initialize& params) = 0;
	virtual void on_shutdown(Params_shutdown& params) = 0;
	virtual void on_textDocument_codeAction(Params_textDocument_codeAction& params) = 0;
	virtual void on_textDocument_codeLens(Params_textDocument_codeLens& params) = 0;
	virtual void on_codeLens_resolve(Params_codeLens_resolve& params) = 0;
	virtual void on_textDocument_completion(Params_textDocument_completion& params) = 0;
	virtual void on_textDocument_definition(Params_text_document_position& params) = 0;
	virtual void on_textDocument_didChange(Params_textDocument_didChange& params) = 0;
	virtual void on_textDocument_didClose(Params_textDocument_didClose& params) = 0;
	virtual void on_textDocument_didOpen(Params_textDocument_didOpen& params) = 0;
	virtual void on_textDocument_didSave(Params_textDocument_didSave& params) = 0;
	virtual void on_textDocument_documentHighlight(Params_text_document_position& params) = 0;
	virtual void on_textDocument_documentSymbol(Params_textDocument_documentSymbol& params) = 0;
	virtual void on_textDocument_formatting(Params_textDocument_formatting& params) = 0;
	virtual void on_textDocument_hover(Params_text_document_position& params) = 0;
	virtual void on_textDocument_implementation(Params_text_document_position& params) = 0;
	virtual void on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting& params) = 0;
	virtual void on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting& params) = 0;
	virtual void on_textDocument_rename(Params_textDocument_rename& params) = 0;
	virtual void on_textDocument_signatureHelp(Params_text_document_position& params) = 0;
	virtual void on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader& params) = 0;
	virtual void on_textDocument_typeDefinition(Params_text_document_position& params) = 0;
	virtual void on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration& params) = 0;
	virtual void on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles& params) = 0;
	virtual void on_workspace_executeCommand(Params_workspace_executeCommand& params) = 0;
};
