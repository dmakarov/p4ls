cmake_minimum_required(VERSION 3.5)

include("cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.23.89.tar.gz" SHA1 "a370290a8b32987755a36d64807bf8c7f5b61730")

project("p4ls")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(UNITTESTS_ENABLED "Build all unit tests." ON)

if (HUNTER_ENABLED)
  hunter_add_package(Boost COMPONENTS date_time filesystem iostreams log regex system thread wave)
  hunter_add_package(RapidJSON)
  find_package(Boost CONFIG REQUIRED COMPONENTS date_time filesystem iostreams log regex system thread wave)
  find_package(RapidJSON CONFIG REQUIRED)
else()
  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost REQUIRED COMPONENTS date_time filesystem iostreams log regex system thread wave)
  find_package(RapidJSON CONFIG REQUIRED)
  add_library(RapidJSON::rapidjson INTERFACE IMPORTED)
  set_target_properties(RapidJSON::rapidjson PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${RAPIDJSON_INCLUDE_DIRS}")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(server)
if (UNITTESTS_ENABLED)
  add_subdirectory(test)
  include(CTest)
endif()
