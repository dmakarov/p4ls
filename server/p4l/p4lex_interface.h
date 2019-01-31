/*
 * -*- c++ -*-
 */

#pragma once

#include <boost/wave/language_support.hpp>
#include <boost/wave/util/file_position.hpp>
#include <boost/wave/cpplexer/cpp_lex_interface.hpp>

#include "p4lex_token.h"

namespace p4l {

/**
 * new_lexer_gen generates a new instance of the required P4 lexer.
 */
template <typename IteratorT, typename PositionT = boost::wave::util::file_position_type>
struct new_lexer_gen {
	/**
	 * The new_lexer function allows the opaque generation of a new
	 * lexer object.  It is coupled to the token type to allow to
	 * decouple the lexer/token configurations at compile time.
	 */
	static boost::wave::cpplexer::lex_input_interface<p4lex_token<PositionT>>*
	new_lexer(IteratorT const& first, IteratorT const& last,
			  PositionT const& pos, boost::wave::language_support language);
};

/**
 * The p4lex_input_interface helps to instantiate a concrete lexer to
 * be used by the Wave preprocessor module.  This is done to allow
 * compile time reduction.
 */
template <typename TokenT>
struct p4lex_input_interface : boost::wave::cpplexer::lex_input_interface<TokenT> {
	using position_type = typename boost::wave::cpplexer::lex_input_interface<TokenT>::position_type;

	~p4lex_input_interface() {}

	/**
	 * The new_lexer function allows the opaque generation of a new
	 * lexer object.  It is coupled to the token type to allow to
	 * distinguish different lexer/token configurations at compile
	 * time.
	*/
	template <typename IteratorT>
	static boost::wave::cpplexer::lex_input_interface<TokenT>*
	new_lexer(IteratorT const& first, IteratorT const& last,
			  position_type const& pos, boost::wave::language_support language) {
		return new_lexer_gen<IteratorT, position_type>::new_lexer(first, last, pos, language);
	}
};

} // namespace p4l
