set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

include_directories(${CMAKE_BINARY_DIR}/p4c ../../vendor/p4c ../../vendor/p4c/test/frameworks/gtest/googletest/include)

add_library(lsp
  context.cpp
  context.h
  dispatcher.cpp
  dispatcher.h
  lsp_server.cpp
  lsp_server.h
  p4unit.cpp
  p4unit.h
  protocol.cpp
  protocol.h)
add_dependencies(lsp ir)
target_link_libraries(lsp RapidJSON::rapidjson)
