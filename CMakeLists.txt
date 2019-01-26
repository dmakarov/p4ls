cmake_minimum_required(VERSION 3.5)

include("cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.20.31.tar.gz" SHA1 "65c8f2292566aedb2fe10e08e09c74dda8e5d07a")

# Customize compiler flags, in particular suppress variadic macro warnings for Boost::test component
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/clang.cmake CACHE FILEPATH "Default toolchain")

project("p4ls")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(UNITTESTS_ENABLED "Build all unit tests." ON)

if (HUNTER_ENABLED)
  hunter_add_package(Boost COMPONENTS date_time system thread filesystem iostreams regex log)
  hunter_add_package(RapidJSON)
  find_package(Boost CONFIG REQUIRED COMPONENTS date_time system thread filesystem iostreams regex log)
  find_package(RapidJSON CONFIG REQUIRED)
else()
  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost REQUIRED COMPONENTS date_time system thread filesystem iostreams regex log)
  find_package(RapidJSON CONFIG REQUIRED)
  add_library(RapidJSON::rapidjson INTERFACE IMPORTED)
  set_target_properties(RapidJSON::rapidjson PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${RAPIDJSON_INCLUDE_DIRS}")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(src)
if (UNITTESTS_ENABLED)
  add_subdirectory(test)
  include(CTest)
endif()
