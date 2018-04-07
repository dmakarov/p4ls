/*
 * -*- c++ -*-
 */

#pragma once

#include <rapidjson/document.h>

class Dispatcher {
public:
	Dispatcher() = default;

	bool call(rapidjson::Document &msg) const;
};
