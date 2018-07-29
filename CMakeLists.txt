cmake_minimum_required(VERSION 3.9)

include("cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.20.31.tar.gz" SHA1 "65c8f2292566aedb2fe10e08e09c74dda8e5d07a")

# Customize compiler flags, in particular suppress variadic macro warnings for Boost::test component
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/clang.cmake CACHE FILEPATH "Default toolchain")

project("p4ls")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UNITTESTS "Build all unit tests." OFF)

if (HUNTER_ENABLED)
    hunter_add_package(Boost COMPONENTS date_time system thread filesystem iostreams regex log)
    hunter_add_package(RapidJSON)
    find_package(Boost CONFIG REQUIRED COMPONENTS date_time system thread filesystem iostreams regex log)
    find_package(RapidJSON CONFIG REQUIRED)
else()
    find_package(Boost REQUIRED COMPONENTS date_time system thread filesystem iostreams regex log)
    find_package(RapidJSON CONFIG REQUIRED)
endif()

include_directories(${RAPIDJSON_INCLUDE_DIRS})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(LibGmp REQUIRED)
include_directories(${LIBGMP_INCLUDE_DIR})

add_subdirectory(vendor/p4c p4c)
add_subdirectory(src)
if (ENABLE_UNITTESTS)
  add_subdirectory(test)
  include(CTest)
endif()
