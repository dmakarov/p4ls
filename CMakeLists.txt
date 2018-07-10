cmake_minimum_required(VERSION 3.9)

include("cmake/HunterGate.cmake")
HunterGate(URL "https://github.com/ruslo/hunter/archive/v0.20.31.tar.gz" SHA1 "65c8f2292566aedb2fe10e08e09c74dda8e5d07a")

# Customize compiler flags, in particular suppress variadic macro warnings for Boost::test component
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/clang.cmake CACHE FILEPATH "Default toolchain")

project("p4ls")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_UNITTESTS "Build all unit tests." OFF)

find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_LIBGMP QUIET gmp)
set(LIBGMP_DEFINITIONS ${PC_LIBGMP_CFLAGS_OTHER})

find_path(LIBGMP_INCLUDE_DIR NAMES gmp.h
    HINTS
    ${PC_LIBGMP_INCLUDEDIR}
    ${PC_LIBGMP_INCLUDE_DIRS}
    )

find_library(LIBGMP_LIBRARY NAMES gmp libgmp
    HINTS
    ${PC_LIBGMP_LIBDIR}
    ${PC_LIBGMP_LIBRARY_DIRS}
    )

find_library(LIBGMPXX_LIBRARY NAMES gmpxx libgmpxx
    HINTS
    ${PC_LIBGMP_LIBDIR}
    ${PC_LIBGMP_LIBRARY_DIRS}
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibGmp
    REQUIRED_VARS LIBGMPXX_LIBRARY LIBGMP_LIBRARY LIBGMP_INCLUDE_DIR
    )
mark_as_advanced(LIBGMP_INCLUDE_DIR LIBGMP_LIBRARY LIBGMPXX_LIBRARY)

if(LIBGMP_FOUND)
    set(LIBGMP_LIBRARIES ${LIBGMP_LIBRARY} ${LIBGMPXX_LIBRARY})
endif()
include_directories(${LIBGMP_INCLUDE_DIR})

add_subdirectory(vendor/p4c p4c)
add_subdirectory(src)
if (ENABLE_UNITTESTS)
  add_subdirectory(test)
  include(CTest)
endif()
