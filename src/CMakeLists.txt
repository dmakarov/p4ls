add_library(p4ls
  lsp_server.cpp)

hunter_add_package(RapidJSON)
find_package(RapidJSON CONFIG REQUIRED)

target_link_libraries(p4ls Boost::boost RapidJSON::rapidjson)

add_subdirectory(tool)
