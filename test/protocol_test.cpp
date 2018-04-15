#include "protocol.h"

#include <boost/test/unit_test.hpp>
#include <rapidjson/istreamwrapper.h>

#include <sstream>


BOOST_AUTO_TEST_SUITE(protocol_test_suite);

BOOST_AUTO_TEST_CASE(test_case1)
{
	std::istringstream input{"{"
							 "\"processId\":20943,"
							 "\"rootPath\":\"/Users/dmakarov/work/try/p4ls/\","
							 "\"rootUri\":\"file:///Users/dmakarov/work/try/p4ls/\","
							 "\"capabilities\":{"
							 "  \"workspace\":{"
							 "    \"applyEdit\":true,"
							 "    \"executeCommand\":{"
							 "      \"dynamicRegistration\":true}},"
							 "  \"textDocument\":{"
							 "    \"synchronization\":{"
							 "      \"willSave\":true,"
							 "      \"didSave\":true},"
							 "    \"documentSymbol\":{"
							 "      \"symbolKind\":{"
							 "        \"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]}}}}}"};
	rapidjson::IStreamWrapper isw(input);
	rapidjson::Document json;
	json.ParseStream(isw);
	Params_initialize params;
	set_params_from_json(json, params);
	BOOST_TEST(params.process_id == 20943);
	BOOST_TEST(params.root_uri == "/Users/dmakarov/work/try/p4ls/");
	BOOST_TEST(*params.capabilities.workspace->apply_edit);
	BOOST_TEST(*params.capabilities.workspace->execute_command->dynamic_registration);
	BOOST_TEST(*params.capabilities.text_document->synchronization->will_save);
	BOOST_TEST(*params.capabilities.text_document->synchronization->did_save);
	std::vector<SYMBOL_KIND> v{SYMBOL_KIND::File,
							   SYMBOL_KIND::Module,
							   SYMBOL_KIND::Namespace,
							   SYMBOL_KIND::Package,
							   SYMBOL_KIND::Class,
							   SYMBOL_KIND::Method,
							   SYMBOL_KIND::Property,
							   SYMBOL_KIND::Field,
							   SYMBOL_KIND::Constructor,
							   SYMBOL_KIND::Enum,
							   SYMBOL_KIND::Interface,
							   SYMBOL_KIND::Function,
							   SYMBOL_KIND::Variable,
							   SYMBOL_KIND::Constant,
							   SYMBOL_KIND::String,
							   SYMBOL_KIND::Number,
							   SYMBOL_KIND::Boolean,
							   SYMBOL_KIND::Array,
							   SYMBOL_KIND::Object,
							   SYMBOL_KIND::Key,
							   SYMBOL_KIND::Null,
							   SYMBOL_KIND::EnumMember,
							   SYMBOL_KIND::Struct,
							   SYMBOL_KIND::Event,
							   SYMBOL_KIND::Operator
	};
	BOOST_TEST(*params.capabilities.text_document->document_symbol->symbol_kind->value_set == v);
}

BOOST_AUTO_TEST_SUITE_END();
