#include "lsp_server.h"

#include <rapidjson/document.h>

#include <iostream>

LSP_server::LSP_server() : _is_done(false)
{
}

int LSP_server::run(std::istream &input_stream)
{
    while (!_is_done && input_stream.good()) {
        if (auto json = read_message(input_stream)) {
        }
    }
    return 0;
}

boost::optional<rapidjson::Document> LSP_server::read_message(std::istream &input_stream)
{
    std::string json_input;
    input_stream >> json_input;
    rapidjson::Document json_document;
    std::cout << "Input is " << json_input << std::endl;
    json_document.Parse(json_input.c_str());
    for (auto& m : json_document.GetObject())
        std::cout << "Type of member " << m.name.GetString()
                  << " is " << m.value.GetType() << std::endl;
    return json_document;
}
