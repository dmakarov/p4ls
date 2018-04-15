/*
 * -*- c++ -*-
 */

#pragma once

#include "dispatcher.h"

#include <rapidjson/document.h>

#include <boost/log/trivial.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <string>
#include <vector>


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

enum class MARKUP_KIND { plaintext, markdown };

enum class Trace_level { OFF = 0, MESSAGES = 1, VERBOSE = 2 };

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

MARKUP_KIND convert_string_to_markup_kind(const std::string &v)
{
	if (v == "plaintext") return MARKUP_KIND::plaintext;
	if (v == "markdown") return MARKUP_KIND::markdown;
	BOOST_LOG_TRIVIAL(error) << "Unknown markup kind " << v;
	return MARKUP_KIND::plaintext;
}

} // namespace

struct URI {
	URI() = default;
	explicit URI(std::string path);

	void set_from_path(const std::string &path)
	{
		_path = std::move(path);
	}

	void set_from_uri(const std::string &uri)
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

	bool operator==(const std::string& other) const
	{
		return _path == other;
	}

	std::string _path;
};

std::ostream &operator<<(std::ostream &os, const URI &item);

struct Workspace_folder {
	Workspace_folder(const rapidjson::Value &json)
	{
	}
	std::string uri;
	std::string name;
};

struct Text_document_client_capabilities {

	explicit Text_document_client_capabilities(const rapidjson::Value &json)
	{
		if (json.HasMember("synchronization") && !json["synchronization"].IsNull())
		{
			synchronization.emplace(Synchronization(json["synchronization"]));
		}
		if (json.HasMember("documentSymbol") && !json["documentSymbol"].IsNull())
		{
			document_symbol.emplace(Document_symbol(json["documentSymbol"]));
		}
	}

	struct Synchronization {
		explicit Synchronization(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("willSave") && !json["willSave"].IsNull())
			{
				will_save.emplace(json["willSave"].GetBool());
			}
			if (json.HasMember("willSaveWaitUntil") && !json["willSaveWaitUntil"].IsNull())
			{
				will_save_wait_until.emplace(json["willSaveWaitUntil"].GetBool());
			}
			if (json.HasMember("didSave") && !json["didSave"].IsNull())
			{
				did_save.emplace(json["didSave"].GetBool());
			}
		}

		boost::optional<bool> dynamic_registration; /// Whether text document synchronization supports dynamic registration.
		boost::optional<bool> will_save;            /// The client supports sending will save notifications.
		boost::optional<bool> will_save_wait_until; /// The client supports sending a will save request and waits for a response providing text edits which will be applied to the document before it is saved.
		boost::optional<bool> did_save;             /// The client supports did save notifications.
	};

	struct Completion {
		struct Completion_item {
			boost::optional<bool> snippetSupport; /// Client supports snippets as insert text. A snippet can define tab stops and placeholders with `$1`, `$2` and `${3:foo}`. `$0` defines the final tab stop, it defaults to the end of the snippet. Placeholders with equal identifiers are linked, that is typing in one will update others too.
			boost::optional<bool> commitCharactersSupport; /// Client supports commit characters on a completion item.
			boost::optional<std::vector<MARKUP_KIND>> documentation_format; /// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
		};

		struct Completion_item_kind {
			boost::optional<std::vector<COMPLETION_ITEM_KIND>> value_set; /// The completion item kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the completion items kinds from `Text` to `Reference` as defined in the initial version of the protocol.
		};

		boost::optional<Completion_item> completion_item; /// The client supports the following `CompletionItem` specific capabilities.
		boost::optional<Completion_item_kind> completion_item_kind;
		boost::optional<bool> dynamic_registration; /// Whether completion supports dynamic registration.
		boost::optional<bool> context_support;		/// The client supports to send additional context information for a `textDocument/completion` request.
	};

	struct Hover {
		bool dynamic_registration; /// Whether hover supports dynamic registration.
		boost::optional<std::vector<MARKUP_KIND>> content_format; /// Client supports the follow content formats for the content property. The order describes the preferred format of the client.
	};

	struct Signature_help {
		struct Signature_information {
			boost::optional<std::vector<MARKUP_KIND>> documentation_format; /// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
		};

		boost::optional<bool> dynamicRegistration; /// Whether signature help supports dynamic registration.
		boost::optional<Signature_information> signature_information; /// The client supports the following `SignatureInformation` specific properties.
	};

	struct References {
		boost::optional<bool> dynamic_registration; /// Whether references supports dynamic registration.
	};

	struct Document_highlight {
		boost::optional<bool> dynamic_registration; /// Whether document highlight supports dynamic registration.
	};

	struct Document_symbol {
		explicit Document_symbol(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("symbolKind") && !json["symbolKind"].IsNull())
			{
				symbol_kind.emplace(Symbol_kind(json["symbolKind"]));
			}
		}

		struct Symbol_kind {
			explicit Symbol_kind(const rapidjson::Value &json)
			{
				if (json.HasMember("valueSet") && !json["valueSet"].IsNull())
				{
					std::vector<SYMBOL_KIND> values;
					for (auto *it = json["valueSet"].Begin(); it != json["valueSet"].End(); ++it)
					{
						values.emplace_back(convert_int_to_symbol_kind(it->GetInt()));
					}
					value_set.emplace(values);
				}
			}

			boost::optional<std::vector<SYMBOL_KIND>> value_set; /// The symbol kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the symbol kinds from `File` to `Array` as defined in the initial version of the protocol.
		};

		boost::optional<bool> dynamic_registration; /// Whether document symbol supports dynamic registration.
		boost::optional<Symbol_kind> symbol_kind; /// Specific capabilities for the `SymbolKind`.
	};

	struct Formatting {
		boost::optional<bool> dynamic_registration; /// Whether formatting supports dynamic registration.
	};

	struct Range_formatting {
		boost::optional<bool> dynamic_registration; /// Whether range formatting supports dynamic registration.
	};

	struct On_type_formatting {
		boost::optional<bool> dynamic_registration; /// Whether on type formatting supports dynamic registration.
	};

	struct Definition {
		boost::optional<bool> dynamic_registration; /// Whether definition supports dynamic registration.
	};

	struct Type_definition {
		boost::optional<bool> dynamic_registration; /// Whether typeDefinition supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Implementation {
		boost::optional<bool> dynamic_registration; /// Whether implementation supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Code_action {
		boost::optional<bool> dynamic_registration; /// Whether code action supports dynamic registration.
	};

	struct Code_lens {
		boost::optional<bool> dynamic_registration; /// Whether code lens supports dynamic registration.
	};

	struct Document_link {
		boost::optional<bool> dynamic_registration; /// Whether document link supports dynamic registration.
	};

	struct Color_provider {
		boost::optional<bool> dynamic_registration; /// Whether colorProvider supports dynamic registration. If this is set to `true` the client supports the new `(ColorProviderOptions & TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
	};

	struct Rename {
		boost::optional<bool> dynamic_registration; /// Whether rename supports dynamic registration.
	};

	struct Publish_diagnostics {
		boost::optional<bool> related_information; /// Whether the clients accepts diagnostics with related information.
	};

	boost::optional<Synchronization> synchronization;
	boost::optional<Completion> completion; /// Capabilities specific to the `textDocument/completion`
	boost::optional<Hover> hover; /// Capabilities specific to the `textDocument/hover`
	boost::optional<Signature_help> signature_help; /// Capabilities specific to the `textDocument/signatureHelp`
	boost::optional<References> references; /// Capabilities specific to the `textDocument/references`
	boost::optional<Document_highlight> document_highlight; /// Capabilities specific to the `textDocument/documentHighlight`
	boost::optional<Document_symbol> document_symbol; /// Capabilities specific to the `textDocument/documentSymbol`
	boost::optional<Formatting> formatting; /// Capabilities specific to the `textDocument/formatting`
	boost::optional<Range_formatting> range_formatting; /// Capabilities specific to the `textDocument/rangeFormatting`
	boost::optional<On_type_formatting> on_type_formatting; /// Capabilities specific to the `textDocument/onTypeFormatting`
	boost::optional<Definition> definition; /// Capabilities specific to the `textDocument/definition`
	boost::optional<Type_definition> type_definition; /// Capabilities specific to the `textDocument/typeDefinition`
	boost::optional<Implementation> implementation; /// Capabilities specific to the `textDocument/implementation`.
	boost::optional<Code_action> code_action; /// Capabilities specific to the `textDocument/codeAction`
	boost::optional<Code_lens> code_lens; /// Capabilities specific to the `textDocument/codeLens`
	boost::optional<Document_link> document_link; /// Capabilities specific to the `textDocument/documentLink`
	boost::optional<Color_provider> color_provider; /// Capabilities specific to the `textDocument/documentColor` and the `textDocument/colorPresentation` request.
	boost::optional<Rename> rename; /// Capabilities specific to the `textDocument/rename`
	boost::optional<Publish_diagnostics> publish_diagnostics; /// Capabilities specific to `textDocument/publishDiagnostics`.
};


struct Workspace_client_capabilities {

	explicit Workspace_client_capabilities(const rapidjson::Value &json)
	{
		if (json.HasMember("applyEdit") && !json["applyEdit"].IsNull())
		{
			apply_edit.emplace(json["applyEdit"].GetBool());
		}
		if (json.HasMember("workspaceFolders") && !json["workspaceFolders"].IsNull())
		{
			workspace_folders.emplace(json["workspaceFolders"].GetBool());
		}
		if (json.HasMember("configuration") && !json["configuration"].IsNull())
		{
			configuration.emplace(json["configuration"].GetBool());
		}
		if (json.HasMember("workspaceEdit") && !json["workspaceEdit"].IsNull())
		{
			workspace_edit.emplace(Workspace_edit(json["workspaceEdit"]));
		}
		if (json.HasMember("didChangeConfiguration") && !json["didChangeConfiguration"].IsNull())
		{
			did_change_configuration.emplace(Did_change_configuration(json["didChangeConfiguration"]));
		}
		if (json.HasMember("didChangeWatchedFiles") && !json["didChangeWatchedFiles"].IsNull())
		{
			did_change_watched_files.emplace(Did_change_watched_files(json["didChangeWatchedFiles"]));
		}
		if (json.HasMember("executeCommand") && !json["executeCommand"].IsNull())
		{
			execute_command.emplace(Execute_command(json["executeCommand"]));
		}
		if (json.HasMember("symbol") && !json["symbol"].IsNull())
		{
			symbol.emplace(Symbol(json["symbol"]));
		}
	}

	struct Workspace_edit {
		explicit Workspace_edit(const rapidjson::Value &json)
		{
			if (json.HasMember("documentChanges") && !json["documentChanges"].IsNull())
			{
				document_changes.emplace(json["documentChanges"].GetBool());
			}
		}
		boost::optional<bool> document_changes; /// The client supports versioned document changes in `WorkspaceEdit`s
	};

	struct Did_change_configuration {
		explicit Did_change_configuration(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> dynamic_registration; /// Did change configuration notification supports dynamic registration.
	};

	struct Did_change_watched_files {
		explicit Did_change_watched_files(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> dynamic_registration; /// Did change watched files notification supports dynamic registration.
	};

	struct Execute_command {
		explicit Execute_command(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
		}
		boost::optional<bool> dynamic_registration; /// execute command supports dynamic registration
	};

	struct Symbol {
		explicit Symbol(const rapidjson::Value &json)
		{
			if (json.HasMember("dynamicRegistration") && !json["dynamicRegistration"].IsNull())
			{
				dynamic_registration.emplace(json["dynamicRegistration"].GetBool());
			}
			if (json.HasMember("symbolKind") && !json["symbolKind"].IsNull())
			{
				symbol_kind.emplace(Symbol_kind(json["symbolKind"]));
			}
		}

		struct Symbol_kind {
			explicit Symbol_kind(const rapidjson::Value &json)
			{
				if (json.HasMember("valueSet") && !json["valueSet"].IsNull())
				{
					std::vector<SYMBOL_KIND> values;
					for (auto *it = json["valueSet"].Begin(); it != json["valueSet"].End(); ++it)
					{
						values.emplace_back(convert_int_to_symbol_kind(it->GetInt()));
					}
					value_set.emplace(values);
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
			boost::optional<std::vector<SYMBOL_KIND>> value_set;
		};

		boost::optional<bool> dynamic_registration; /// Symbol request supports dynamic registration.
		boost::optional<Symbol_kind> symbol_kind;   /// Specific capabilities for the `SymbolKind` in the `workspace/symbol` request
	};

	boost::optional<Workspace_edit> workspace_edit;                     /// capabilities specific to `WorkspaceEdit`s
	boost::optional<Did_change_configuration> did_change_configuration; /// capabilities specific to the `workspace/didChangeConfiguration` notification
	boost::optional<Did_change_watched_files> did_change_watched_files; /// capabilities specific to the `workspace/didChangeWatchedFiles` notification
	boost::optional<Execute_command> execute_command;                   /// capabilities specific to the `workspace/executeCommand` request
	boost::optional<Symbol> symbol;                                     /// capabilities specific to the `workspace/symbol` request
	boost::optional<bool> apply_edit;        /// support applying batch edits to the workspace
	boost::optional<bool> workspace_folders; /// support workspace folders
	boost::optional<bool> configuration;     /// support `workspace/configuration` requests
};


struct Client_capabilities {

	void set(const rapidjson::Value &json)
	{
		if (json.HasMember("workspace") && !json["workspace"].IsNull())
		{
			workspace.emplace(Workspace_client_capabilities{std::move(json["workspace"])});
		}
		if (json.HasMember("textDocument") && !json["textDocument"].IsNull())
		{
			text_document.emplace(Text_document_client_capabilities{std::move(json["textDocument"])});
		}
	}

	boost::optional<Workspace_client_capabilities> workspace;
	boost::optional<Text_document_client_capabilities> text_document;
};

struct Server_capabilities {

	struct Save_options {
		bool include_text; /// the client is supposed to include the content on save
	};

	struct Text_document_sync_options {
		enum Text_document_sync_kind {
			None = 0, /// documents should not be synced at all
			Full = 1, /// documents are synced by always sending the full content of the document
			Incremental = 2 /// documents are synced by sending the full content on open; after that only incremental updates to the document are send
		};
		bool open_close;           /// open and close notifications are sent to the server
		bool will_save;            /// will save notifications are sent to the server
		bool will_save_wait_until; /// will save wait until requests are sent to the server
		Save_options save;         /// save notifications are sent to the server
		Text_document_sync_kind change; /// change notifications are sent to the server
	};

	struct Completion_options {
		bool resolve_provider;          /// the server provides support to resolve additional information for a completion item
		std::string trigger_characters; /// the characters that trigger completion automatically.
	};

	struct Signature_help_options {
		std::string trigger_characters; /// the characters that trigger signature help automatically
	};

	struct Code_lens_options {
		bool resolve_provider; /// code lens has a resolve provider as well
	};

	struct Document_on_type_formatting_options {
		std::string first_trigger_character; /// A character on which formatting should be triggered, like `}`
		std::string more_trigger_character; /// More trigger characters.
	};

	struct Document_link_options {
		bool resolve_provider; /// Document links have a resolve provider as well.
	};

	struct Execute_command_options {
		std::vector<std::string> commands; ///  The commands to be executed on the server
	};

	struct Workspace_folders {
		bool supported; /// The server has support for workspace folders
		std::string change_notifications; /// Whether the server wants to receive workspace folder change notifications. If a strings is provided the string is treated as a ID under which the notification is registed on the client side. The ID can be used to unregister for these events using the `client/unregisterCapability` request.
	};

	struct Color_provider_options {
	};

	Text_document_sync_options text_document_sync;
	bool hover_provider;
	Completion_options completion_provider;
	Signature_help_options signature_help_provider;
	bool definition_provider;
	bool type_definition_provider;
	bool implementation_provider;
	bool references_provider;
	bool document_highlight_provider;
	bool document_symbol_provider; /// The server provides document symbol support.
	bool workspace_symbol_provider; /// The server provides workspace symbol support.
	bool code_action_provider; /// The server provides code actions.
	Code_lens_options code_lens_provider; /// The server provides code lens.
	bool document_formatting_provider; /// The server provides document formatting.
	bool document_range_formatting_provider; /// The server provides document range formatting.
	Document_on_type_formatting_options document_on_type_formatting_provider; /// The server provides document formatting on typing.
	bool rename_provider; /// The server provides rename support.
	Document_link_options document_link_provider; /// The server provides document link support.
	Color_provider_options color_provider; /// The server provides color provider support.
	Execute_command_options execute_command_provider; /// The server provides execute command support.
	Workspace_folders workspace; /// Workspace specific server capabilities
};

struct Params_exit {
};

bool set_params_from_json(const rapidjson::Value &json, Params_exit &params);

struct Params_initialize {
	int process_id;
	URI root_uri;
	Client_capabilities capabilities;
	boost::optional<Trace_level> trace;
	boost::optional<std::vector<std::string>> initialization_options;
	boost::optional<std::vector<Workspace_folder>> workspace_folders;
};

bool set_params_from_json(const rapidjson::Value &json, Params_initialize &params);

struct Params_shutdown {
};

bool set_params_from_json(const rapidjson::Value &json, Params_shutdown &params);

struct Params_textDocument_codeAction {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_codeAction &params);

struct Params_textDocument_completion {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_completion &params);

struct Params_textDocument_definition {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_definition &params);

struct Params_textDocument_didChange {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didChange &params);

struct Params_textDocument_didClose {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didClose &params);

struct Params_textDocument_didOpen {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_didOpen &params);

struct Params_textDocument_documentHighlight {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_documentHighlight &params);

struct Params_textDocument_formatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_formatting &params);

struct Params_textDocument_hover {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_hover &params);

struct Params_textDocument_onTypeFormatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_onTypeFormatting &params);

struct Params_textDocument_rangeFormatting {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rangeFormatting &params);

struct Params_textDocument_rename {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_rename &params);

struct Params_textDocument_signatureHelp {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_signatureHelp &params);

struct Params_textDocument_switchSourceHeader {
};

bool set_params_from_json(const rapidjson::Value &json, Params_textDocument_switchSourceHeader &params);

struct Params_workspace_didChangeConfiguration {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeConfiguration &params);

struct Params_workspace_didChangeWatchedFiles {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_didChangeWatchedFiles &params);

struct Params_workspace_executeCommand {
};

bool set_params_from_json(const rapidjson::Value &json, Params_workspace_executeCommand &params);

class Protocol {
public:
	virtual ~Protocol() = default;

	virtual void on_exit(Params_exit &params) = 0;
	virtual void on_initialize(Params_initialize &params) = 0;
	virtual void on_shutdown(Params_shutdown &params) = 0;
	virtual void on_textDocument_codeAction(Params_textDocument_codeAction &params) = 0;
	virtual void on_textDocument_completion(Params_textDocument_completion &params) = 0;
	virtual void on_textDocument_definition(Params_textDocument_definition &params) = 0;
	virtual void on_textDocument_didChange(Params_textDocument_didChange &params) = 0;
	virtual void on_textDocument_didClose(Params_textDocument_didClose &params) = 0;
	virtual void on_textDocument_didOpen(Params_textDocument_didOpen &params) = 0;
	virtual void on_textDocument_documentHighlight(Params_textDocument_documentHighlight &params) = 0;
	virtual void on_textDocument_formatting(Params_textDocument_formatting &params) = 0;
	virtual void on_textDocument_hover(Params_textDocument_hover &params) = 0;
	virtual void on_textDocument_onTypeFormatting(Params_textDocument_onTypeFormatting &params) = 0;
	virtual void on_textDocument_rangeFormatting(Params_textDocument_rangeFormatting &params) = 0;
	virtual void on_textDocument_rename(Params_textDocument_rename &params) = 0;
	virtual void on_textDocument_signatureHelp(Params_textDocument_signatureHelp &params) = 0;
	virtual void on_textDocument_switchSourceHeader(Params_textDocument_switchSourceHeader &params) = 0;
	virtual void on_workspace_didChangeConfiguration(Params_workspace_didChangeConfiguration &params) = 0;
	virtual void on_workspace_didChangeWatchedFiles(Params_workspace_didChangeWatchedFiles &params) = 0;
	virtual void on_workspace_executeCommand(Params_workspace_executeCommand &params) = 0;
};

void register_protocol_handlers(Dispatcher &dispatcher, Protocol &protocol);
