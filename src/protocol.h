/*
 * -*- c++ -*-
 */

#pragma once

#include "dispatcher.h"

#include <rapidjson/document.h>
#include <boost/optional.hpp>

#include <string>
#include <vector>

enum class Trace_level {
	OFF = 0, MESSAGES = 1, VERBOSE = 2
};

struct File_uri {
	File_uri() = default;
	explicit File_uri(std::string path);

	void set_from_path(const std::string &path)
	{
		_path = std::move(path);
	}

	void set_from_uri(const std::string &uri)
	{
	}

private:
	std::string _path;
};

struct Workspace_folder {
	Workspace_folder(const rapidjson::Value &json)
	{
	}
	std::string uri;
	std::string name;
};

enum Symbol_kind {
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

struct Client_capabilities {

	struct Workspace_client_capabilities {
		bool applyEdit; /// the client supports applying batch edits to the workspace by supporting the request 'workspace/applyEdit'
		bool workspace_edit_document_changes; /// the client supports versioned document changes in `WorkspaceEdit`s
		bool did_change_configuration_dynamic_registration; /// did change configuration notification supports dynamic registration
		bool did_change_watched_files_dynamic_registration; /// did change watched files notification supports dynamic registration
		bool symbol_dynamic_registration; /// symbol request supports dynamic registration
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
		std::vector<Symbol_kind> symbol_value_set;
		bool execute_command_dynamic_registration; /// execute command supports dynamic registration
		bool workspace_Folders; /// the client has support for workspace folders
		bool configuration; /// the client supports `workspace/configuration` requests
	};


	struct Text_document_client_capabilities {
		/*
	synchronization?: {
		/// Whether text document synchronization supports dynamic registration.
		bool dynamicRegistration;

		/// The client supports sending will save notifications.
		bool willSave;

		/// The client supports sending a will save request and waits for a response providing text edits which will be applied to the document before it is saved.
		bool willSaveWaitUntil;

		/// The client supports did save notifications.
		bool didSave;
	}

	/// Capabilities specific to the `textDocument/completion`
	completion?: {
		/// Whether completion supports dynamic registration.
		bool dynamicRegistration;

		/// The client supports the following `CompletionItem` specific capabilities.
		completionItem?: {
			/// Client supports snippets as insert text. A snippet can define tab stops and placeholders with `$1`, `$2` and `${3:foo}`. `$0` defines the final tab stop, it defaults to the end of the snippet. Placeholders with equal identifiers are linked, that is typing in one will update others too.
			bool snippetSupport;

			/// Client supports commit characters on a completion item.
			bool commitCharactersSupport

			/// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
			documentationFormat?: MarkupKind[];
		}

		completionItemKind?: {
			/// The completion item kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the completion items kinds from `Text` to `Reference` as defined in the initial version of the protocol.
			valueSet?: CompletionItemKind[];
		},

		/// The client supports to send additional context information for a `textDocument/completion` request.
		bool contextSupport;
	};

	/// Capabilities specific to the `textDocument/hover`
	hover?: {
		/// Whether hover supports dynamic registration.
		bool dynamicRegistration;

		/// Client supports the follow content formats for the content property. The order describes the preferred format of the client.
		contentFormat?: MarkupKind[];
	};

	/// Capabilities specific to the `textDocument/signatureHelp`
	signatureHelp?: {
		/// Whether signature help supports dynamic registration.
		bool dynamicRegistration;

		/// The client supports the following `SignatureInformation` specific properties.
		signatureInformation?: {
			/// Client supports the follow content formats for the documentation property. The order describes the preferred format of the client.
			documentationFormat?: MarkupKind[];
		};
	};

	/// Capabilities specific to the `textDocument/references`
	references?: {
		/// Whether references supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/documentHighlight`
	documentHighlight?: {
		/// Whether document highlight supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/documentSymbol`
	documentSymbol?: {
		/// Whether document symbol supports dynamic registration.
		bool dynamicRegistration;

		/// Specific capabilities for the `SymbolKind`.
		symbolKind?: {
			/// The symbol kind values the client supports. When this property exists the client also guarantees that it will handle values outside its set gracefully and falls back to a default value when unknown. If this property is not present the client only supports the symbol kinds from `File` to `Array` as defined in the initial version of the protocol.
			valueSet?: SymbolKind[];
		}
	};

	/// Capabilities specific to the `textDocument/formatting`
	formatting?: {
		/// Whether formatting supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/rangeFormatting`
	rangeFormatting?: {
		/// Whether range formatting supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/onTypeFormatting`
	onTypeFormatting?: {
		/// Whether on type formatting supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/definition`
	definition?: {
		/// Whether definition supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/typeDefinition`
	typeDefinition?: {
		/// Whether typeDefinition supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/implementation`.
	implementation?: {
		/// Whether implementation supports dynamic registration. If this is set to `true` the client supports the new `(TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/codeAction`
	codeAction?: {
		/// Whether code action supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/codeLens`
	codeLens?: {
		/// Whether code lens supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/documentLink`
	documentLink?: {
		/// Whether document link supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to the `textDocument/documentColor` and the `textDocument/colorPresentation` request.
	colorProvider?: {
		/// Whether colorProvider supports dynamic registration. If this is set to `true` the client supports the new `(ColorProviderOptions & TextDocumentRegistrationOptions & StaticRegistrationOptions)` return value for the corresponding server capability as well.
		bool dynamicRegistration;
	}

	/// Capabilities specific to the `textDocument/rename`
	rename?: {
		/// Whether rename supports dynamic registration.
		bool dynamicRegistration;
	};

	/// Capabilities specific to `textDocument/publishDiagnostics`.
	publishDiagnostics?: {
		/// Whether the clients accepts diagnostics with related information.
		bool relatedInformation;
	};
	*/
	};

	Client_capabilities()
	{
	}

	void set(const rapidjson::Value &json)
	{
	}

private:
	Workspace_client_capabilities workspace;
	Text_document_client_capabilities text_document;
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
		/// The server has support for workspace folders
		bool supported;
		/// Whether the server wants to receive workspace folder change notifications. If a strings is provided the string is treated as a ID under which the notification is registed on the client side. The ID can be used to unregister for these events using the `client/unregisterCapability` request.
		std::string changeNotifications;
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
	File_uri root_uri;
	Client_capabilities capabilities;
	Trace_level trace;
	boost::optional<std::string> options;
	std::vector<Workspace_folder> workspace_folders;
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
