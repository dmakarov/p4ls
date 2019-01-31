#include <boost/pool/pool_alloc.hpp>
#include <boost/wave/grammars/cpp_chlit_grammar.hpp>
#include <boost/wave/grammars/cpp_defined_grammar.hpp>
#include <boost/wave/grammars/cpp_expression_grammar.hpp>
#include <boost/wave/grammars/cpp_grammar.hpp>
#include <boost/wave/grammars/cpp_intlit_grammar.hpp>
#include <boost/wave/grammars/cpp_literal_grammar_gen.hpp>

#include <list>

#include "p4lex_token.h"
#include "p4lex_iterator.h"

using token_type = p4l::p4lex_token<>;
using lexer_type = p4l::p4lex_iterator<token_type>;
using token_sequence_type = std::list<token_type, boost::fast_pool_allocator<token_type>>;
/**
 * Explicit instantiation of the cpp_grammar_gen template with the
 * correct token type. This instantiates the corresponding pt_parse
 * function, which in turn instantiates the cpp_grammar object (see
 * wave/grammars/cpp_grammar.hpp)
 */
template struct boost::wave::grammars::cpp_grammar_gen<lexer_type, token_sequence_type>;
/**
 * Explicit instantiation of the defined_grammar_gen template with the
 * correct token type. This instantiates the corresponding parse
 * function, which in turn instantiates the defined_grammar object
 * (see wave/grammars/cpp_defined_grammar.hpp)
 */
template struct boost::wave::grammars::defined_grammar_gen<lexer_type>;
/**
 * Explicit instantiation of the expression_grammar_gen template with
 * the correct token type. This instantiates the corresponding parse
 * function, which in turn instantiates the expression_grammar object
 * (see wave/grammars/cpp_expression_grammar.hpp)
 */
template struct boost::wave::grammars::expression_grammar_gen<token_type>;
/**
 * Explicit instantiation of the intlit_grammar_gen, chlit_grammar_gen
 * and floatlit_grammar_gen templates with the correct token
 * type. This instantiates the corresponding parse function, which in
 * turn instantiates the corresponding parser object.
 */
template struct boost::wave::grammars::chlit_grammar_gen<int, token_type>;
template struct boost::wave::grammars::chlit_grammar_gen<unsigned int, token_type>;
template struct boost::wave::grammars::intlit_grammar_gen<token_type>;
/**
 * This instantiates the correct 'new_lexer' function, which generates
 * the P4 lexer.

 * This is moved into a separate compilation unit to decouple the
 * compilation of the C++ lexer from the compilation of the other
 * modules, which helps to reduce compilation time.

 * The template parameter(s) supplied should be identical to the
 * parameters supplied while instantiating the context<> template.
 */
template struct p4l::new_lexer_gen<std::string::iterator>;
