add_library(p4ls
  lsp_server.cpp)

target_link_libraries(p4ls Boost::boost)

add_subdirectory(tool)
