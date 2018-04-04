cmake_minimum_required(VERSION 3.9)

include("cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.20.31.tar.gz" SHA1 "65c8f2292566aedb2fe10e08e09c74dda8e5d07a")

project("p4ls")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_UNITTESTS "Build all unit tests." OFF)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/clang.cmake)
endif()

add_subdirectory(src)
if (ENABLE_UNITTESTS)
  add_subdirectory(test)
endif()
