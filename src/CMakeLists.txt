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

hunter_add_package(Boost COMPONENTS date_time system thread filesystem iostreams regex log)
hunter_add_package(RapidJSON)
find_package(Boost CONFIG REQUIRED COMPONENTS date_time system thread filesystem iostreams regex log)
find_package(RapidJSON CONFIG REQUIRED)

find_library(LIBCONTROLPLANE_LIBRARY controlplane HINTS /Users/dmakarov/work/try/p4c/build/ninja/control-plane)
find_library(LIBFRONTEND_LIBRARY frontend HINTS /Users/dmakarov/work/try/p4c/build/ninja/frontends)
find_library(LIBIR_LIBRARY ir HINTS /Users/dmakarov/work/try/p4c/build/ninja/ir)
find_library(LIBMIDEND_LIBRARY midend HINTS /Users/dmakarov/work/try/p4c/build/ninja/midend)
find_library(LIBP4CTOOLKIT_LIBRARY p4ctoolkit HINTS /Users/dmakarov/work/try/p4c/build/ninja/lib)

set (P4C_LIB_DEPS "${Boost_LIBRARIES};${LIBGMP_LIBRARIES};${LIBGC_LIBRARIES}")
set (P4C_LIBRARIES
  ${LIBCONTROLPLANE_LIBRARY}
  ${LIBMIDEND_LIBRARY}
  ${LIBFRONTEND_LIBRARY}
  ${LIBIR_LIBRARY}
  ${LIBP4CTOOLKIT_LIBRARY})

message("P4C_LIB_DEPS is ${P4C_LIB_DEPS}")

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
