add_library(p4ls
  dispatcher.cpp
  dispatcher.h
  lsp_server.cpp
  lsp_server.h
  params.h
  protocol.cpp
  protocol.h)

hunter_add_package(Boost)
hunter_add_package(RapidJSON)
find_package(Boost CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

target_link_libraries(p4ls PUBLIC Boost::boost RapidJSON::rapidjson)

add_subdirectory(tool)
