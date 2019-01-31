/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/wave/wave_config.hpp>
#include <boost/wave/token_ids.hpp>
#include <boost/wave/language_support.hpp>
#include <boost/wave/util/file_position.hpp>

#include <iomanip>
#include <ostream>

namespace p4l {

template <typename PositionT = boost::wave::util::file_position_type>
class p4lex_token;

template <typename PositionT>
class p4lex_token {
 public:
	using string_type = BOOST_WAVE_STRINGTYPE;
	using position_type = PositionT;

	p4lex_token()
		: _id(boost::wave::T_EOI)
	{}

	explicit p4lex_token(int)
		: _id(boost::wave::T_UNKNOWN)
	{}

	p4lex_token(boost::wave::token_id id, string_type const& value, PositionT const& pos)
		: _id(id), _value(value), _pos(pos)
	{}

	operator boost::wave::token_id() const noexcept {
		return _id;
	}

	string_type const& get_value() const noexcept {
		return _value;
	}

	position_type const& get_position() const noexcept {
		return _pos;
	}

	bool is_eoi() const noexcept {
		return _id == boost::wave::T_EOI;
	}

	bool is_valid() const noexcept {
		return _id != boost::wave::T_UNKNOWN;
	}

	void set_token_id(boost::wave::token_id id) noexcept {
		_id = id;
	}

	void set_value(string_type const& newval) noexcept {
		_value = newval;
	}

	void set_position(position_type const& pos) noexcept {
		_pos = pos;
	}

	// tokens are considered equal even if they are at different positions
	friend bool operator==(p4lex_token const& lhs, p4lex_token const& rhs) {
		return lhs._id == rhs._id && lhs._value == rhs._value;
    }

	void print (std::ostream& stream) const {
		stream << std::setw(16)
			   << std::left << boost::wave::get_token_name(_id)
			   << " (" << "#" << boost::wave::token_id(BASEID_FROM_TOKEN(*this))
			   << ") at " << get_position().get_file() << " ("
			   << std::setw(3) << std::right << get_position().get_line() << "/"
			   << std::setw(2) << std::right << get_position().get_column()
			   << "): >";

		for (auto& c : _value) {
			switch (c) {
			case '\r': stream << "\\r"; break;
			case '\n': stream << "\\n"; break;
			case '\t': stream << "\\t"; break;
			default:   stream << c;
			}
		}
		stream << "<";
	}

 private:
	boost::wave::token_id _id;
	string_type _value;
	position_type _pos;
};

template <typename PositionT>
inline std::ostream& operator<<(std::ostream& stream, p4lex_token<PositionT> const& object)
{
	object.print(stream);
	return stream;
}

/**
 * This overload is needed by the multi_pass/functor_input_policy to
 * validate a token instance.  It has to be defined in the same
 * namespace as the token class itself to allow ADL to find it.
*/
template <typename Position>
inline bool token_is_valid(p4lex_token<Position> const& t) {
	return t.is_valid();
}

}  // namespace p4l
