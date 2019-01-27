add_library(p4l
  lexer.cpp
  lexer.h)

target_compile_options(p4l PRIVATE
  "-Wall"
  "-Werror"
  "-Wextra"
  "-fvisibility=hidden"
  "-fvisibility-inlines-hidden")

target_link_libraries(p4l Boost::boost)
