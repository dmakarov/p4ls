/*
 * -*- c++ -*-
 */

#pragma once

#include <istream>

#include <boost/optional.hpp>

class LSP_server
{
 public:
    LSP_server();
    int run();

 private:
    boost::optional<std::string> read_message(std::istream &input_stream);

};
