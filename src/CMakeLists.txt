set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

add_library(p4ls
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

set (P4C_LIB_DEPS "${Boost_LIBRARIES};${LIBGMP_LIBRARIES}")
set (P4C_LIBRARIES p4ctoolkit frontend ir)

include_directories(${CMAKE_BINARY_DIR}/p4c ../vendor/p4c ../vendor/p4c/test/frameworks/gtest/googletest/include)

target_link_libraries(p4ls
  PUBLIC
  Boost::boost
  Boost::log
  Boost::regex
  Boost::filesystem
  Boost::iostreams
  Boost::date_time
  Boost::thread
  Boost::system
  RapidJSON::rapidjson
  ${P4C_LIBRARIES}
  ${P4C_LIB_DEPS})

add_subdirectory(tool)
