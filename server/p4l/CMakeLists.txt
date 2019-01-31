add_library(p4l
  instances.cpp
  lexer.cpp
  lexer.h
  p4lex_iterator.h
  p4lex_interface.h
  p4lex_token.h)

target_compile_options(p4l PRIVATE
  "-g"
  "-Wall"
  "-Werror"
  "-Wextra"
  "-fvisibility=hidden"
  "-fvisibility-inlines-hidden")

target_link_libraries(p4l PUBLIC
  coverage_config
  Boost::boost)
