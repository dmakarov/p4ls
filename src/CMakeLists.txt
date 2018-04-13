set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

add_library(p4ls
  context.cpp
  context.h
  dispatcher.cpp
  dispatcher.h
  lsp_server.cpp
  lsp_server.h
  protocol.cpp
  protocol.h)

hunter_add_package(Boost COMPONENTS date_time system thread filesystem regex log)
hunter_add_package(RapidJSON)
find_package(Boost CONFIG REQUIRED COMPONENTS date_time system thread filesystem regex log)
find_package(RapidJSON CONFIG REQUIRED)

target_link_libraries(p4ls PUBLIC Boost::boost Boost::log Boost::regex Boost::filesystem Boost::date_time Boost::thread Boost::system RapidJSON::rapidjson)

add_subdirectory(tool)
