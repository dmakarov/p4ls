#include "lsp_server.h"

#include <rapidjson/document.h>

LSP_server::LSP_server()
{
}

int LSP_server::run()
{
    return 0;
}

boost::optional<std::string> LSP_server::read_message(std::istream &input_stream)
{
    const char* json_input = "{\"project\":\"rapidjson\",\"stars\":10}";
    rapidjson::Document json_document;
    json_document.Parse(json_input);
    return {};
}
