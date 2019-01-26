add_library(p4l
  lexer.cpp
  lexer.h)

target_compile_options(p4l PRIVATE "-Werror" "-Wall" "-Wextra" "-fvisibility=hidden" "-fvisibility-inlines-hidden")
