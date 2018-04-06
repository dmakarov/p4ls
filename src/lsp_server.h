/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/optional.hpp>
#include <rapidjson/document.h>

#include <istream>

class LSP_server
{
 public:
    LSP_server();
    int run(std::istream &input_stream);

 private:
    boost::optional<rapidjson::Document> read_message(std::istream &input_stream);

    bool _is_done;
};
