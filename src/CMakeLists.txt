add_library(p4ls
  dispatcher.cpp
  dispatcher.h
  lsp_server.cpp
  lsp_server.h
  params.cpp
  params.h
  protocol.cpp
  protocol.h)

hunter_add_package(Boost COMPONENTS date_time system thread filesystem log)
hunter_add_package(RapidJSON)
find_package(Boost CONFIG REQUIRED COMPONENTS date_time system thread filesystem log)
find_package(RapidJSON CONFIG REQUIRED)

target_link_libraries(p4ls PUBLIC Boost::boost Boost::date_time Boost::system Boost::thread Boost::filesystem Boost::log RapidJSON::rapidjson)

add_subdirectory(tool)
