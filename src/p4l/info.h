/*
 * -*- c++ -*-
 */

#pragma once

#include <rapidjson/document.h>


struct Position {
	Position() : _line(0), _character(0)
	{}

	Position(unsigned int line, unsigned int column)
		: _line(line)
		, _character(column)
	{}

	Position(const rapidjson::Value& json)
		: _line(json["line"].GetInt())
		, _character(json["character"].GetInt())
	{}

	rapidjson::Value get_json(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value result(rapidjson::kObjectType);
		result.AddMember("line", _line, allocator);
		result.AddMember("character", _character, allocator);
		return result;
	}

	bool set(const rapidjson::Value& json)
	{
		if (json.HasMember("line") && json.HasMember("character"))
		{
			_line = json["line"].GetInt();
			_character = json["character"].GetInt();
			return true;
		}
		return false;
	}

	unsigned int get_line_number() const noexcept {
		return _line;
	}

	unsigned int get_column_number() const noexcept {
		return _character;
	}

	unsigned int _line;	// Line position in a document (zero-based).

	/**
	 * Character offset on a line in a document (zero-based). Assuming
	 * that the line is represented as a string, the `character` value
	 * represents the gap between the `character` and `character + 1`.
	 *
	 * If the character value is greater than the line length it
	 * defaults back to the line length.
	 */
	unsigned int _character;
};


class Info {
 public:
	Position get_start() const noexcept {
		return start;
	}
	Position get_end() const noexcept {
		return end;
	}
	Position start;
	Position end;
};
