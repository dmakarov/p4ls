set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

add_library(p4l
  lexer.cpp
  lexer.h)
