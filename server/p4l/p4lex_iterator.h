/*
 * -*- c++ -*-
 */

#pragma once

#include <istream>
#include <string>
#include <iostream>

#include <boost/spirit/include/support_multi_pass.hpp>

#include <boost/wave/language_support.hpp>
#include <boost/wave/util/file_position.hpp>
#include <boost/wave/util/functor_input.hpp>

#include "p4lex_interface.h"

namespace p4l {

namespace impl {

template <typename TokenT>
class p4lex_iterator_functor_shim {

	using position_type = typename TokenT::position_type;

 public:
	p4lex_iterator_functor_shim() = default;

	using result_type = TokenT;
	using unique = p4lex_iterator_functor_shim;
	using shared = boost::wave::cpplexer::lex_input_interface<TokenT>*;

	static result_type const eof;

	template <typename MultiPass>
	static result_type& get_next(MultiPass& mp, result_type& result) {
		return mp.shared()->ftor->get(result);
	}

	// this will be called whenever the last reference to a multi_pass will
	// be released
	template <typename MultiPass>
	static void destroy(MultiPass& mp) {
		delete mp.shared()->ftor;
	}

	template <typename MultiPass>
	static void set_position(MultiPass& mp, position_type const& pos) {
		mp.shared()->ftor->set_position(pos);
	}

	template <typename MultiPass>
	static bool has_include_guards(MultiPass& mp, std::string& guard_name) {
		return mp.shared()->ftor->has_include_guards(guard_name);
	}
};

template <typename TokenT>
typename p4lex_iterator_functor_shim<TokenT>::result_type const
p4lex_iterator_functor_shim<TokenT>::eof = typename p4lex_iterator_functor_shim<TokenT>::result_type();

}  // namespace impl

template <typename FunctorData>
struct make_multi_pass {
	using functor_data_type = std::pair<typename FunctorData::unique, typename FunctorData::shared>;
	using result_type = typename FunctorData::result_type;
	using input_policy = boost::spirit::iterator_policies::split_functor_input;
	using ownership_policy = boost::spirit::iterator_policies::ref_counted;
	using check_policy = boost::spirit::iterator_policies::no_check;
	using storage_policy = boost::spirit::iterator_policies::split_std_deque;
	using policy_type = boost::spirit::iterator_policies::default_policy<ownership_policy, check_policy, input_policy, storage_policy>;
	using type = boost::spirit::multi_pass<functor_data_type, policy_type>;
};

/**
 * p4lex_iterator
 *
 *     A generic C++ lexer interface class, which allows to plug in different
 *     lexer implementations (template parameter LexT). The following
 *     requirement apply:
 *
 *         - the lexer type should have a function implemented, which returns
 *           the next lexed token from the input stream:
 *               typename LexT::token_type get();
 *         - at the end of the input stream this function should return the
 *           eof token equivalent
 *         - the lexer should implement a constructor taking two iterators
 *           pointing to the beginning and the end of the input stream and
 *           a third parameter containing the name of the parsed input file,
 *           the 4th parameter contains the information about the mode the
 *           preprocessor is used in (C99/C++ mode etc.)
 *
 * Divide the given functor type into its components (unique and shared)
 * and build a std::pair from these parts
 */
template <typename TokenT>
class p4lex_iterator : public make_multi_pass<impl::p4lex_iterator_functor_shim<TokenT>>::type {
	using input_policy_type = impl::p4lex_iterator_functor_shim<TokenT>;
	using base_type = typename make_multi_pass<input_policy_type>::type;
	using functor_data_type = typename make_multi_pass<input_policy_type>::functor_data_type;
	using unique_functor_type = typename input_policy_type::unique;
	using shared_functor_type = typename input_policy_type::shared;

 public:
	using token_type = TokenT;
	using iterator_category = std::forward_iterator_tag;

	p4lex_iterator() = default;

	template <typename IteratorT>
	p4lex_iterator(IteratorT const &first, IteratorT const &last,
				   typename TokenT::position_type const &pos,
				   boost::wave::language_support language)
		: base_type(functor_data_type(unique_functor_type(),
									  p4lex_input_interface<TokenT>::new_lexer(first, last, pos, language)))
	{}

	void set_position(typename TokenT::position_type const &pos) {
		// set the new position in the current token
		auto &currtoken = this->base_type::dereference(*this);
		auto currpos = currtoken.get_position();

		currpos.set_file(pos.get_file());
		currpos.set_line(pos.get_line());
		const_cast<token_type&>(currtoken).set_position(currpos);

		// set the new position for future tokens as well
		if (token_type::string_type::npos != currtoken.get_value().find_first_of('\n')) {
			currpos.set_line(pos.get_line() + 1);
		}
		unique_functor_type::set_position(*this, currpos);
	}

	bool has_include_guards(std::string& guard_name) const {
		return unique_functor_type::has_include_guards(*this, guard_name);
	}
};

} // namespace p4l
