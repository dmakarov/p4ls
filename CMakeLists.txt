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

PKG_CHECK_MODULES(PC_LIBGC QUIET bwd-gc)
set(LIBGC_DEFINITIONS ${PC_LIBGC_CFLAGS_OTHER})

find_path(LIBGC_INCLUDE_DIR NAMES gc/gc.h
    HINTS
    ${PC_LIBGC_INCLUDEDIR}
    ${PC_LIBGC_INCLUDE_DIRS}
    PATH_SUFFIXES gc
    )

find_library(LIBGC_LIBRARIES NAMES gc libgc
    HINTS
    ${PC_LIBGC_LIBDIR}
    ${PC_LIBGC_LIBRARY_DIRS}
    )

if(PC_LIBGC_VERSION)
    set(LIBGC_VERSION_STRING ${PC_LIBGC_VERSION})
elseif(LIBGC_INCLUDE_DIR AND EXISTS "${LIBGC_INCLUDE_DIR}/gc/gc_version.h")
    file(STRINGS "${LIBGC_INCLUDE_DIR}/gc/gc_version.h" libgc_version_major
        REGEX "^#define[\t ]+GC_TMP_VERSION_MAJOR[\t ]+[0-9]+.*")
    file(STRINGS "${LIBGC_INCLUDE_DIR}/gc/gc_version.h" libgc_version_minor
        REGEX "^#define[\t ]+GC_TMP_VERSION_MINOR[\t ]+[0-9]+.*")
    file(STRINGS "${LIBGC_INCLUDE_DIR}/gc/gc_version.h" libgc_version_patch
        REGEX "^#define[\t ]+GC_TMP_VERSION_MICRO[\t ]+[0-9]+.*")

    # some earlier versions do not put the full version as a comment
    if (${libgc_version_patch})
      string(REGEX REPLACE "^#define[\t ]+GC_TMP_VERSION_MICRO[\t 0-9]+/\\*[\t ]+([0-9\\.]+)[\t ]+\\*/" "\\1"
        LIBGC_VERSION_STRING "${libgc_version_patch}")
    else()
      string(REGEX REPLACE "^#define[\t ]+GC_TMP_VERSION_MAJOR[\t ]+([0-9]+)" "\\1"
        libgc_version_major "${libgc_version_major}")
      string(REGEX REPLACE "^#define[\t ]+GC_TMP_VERSION_MINOR[\t ]+([0-9]+)" "\\1"
        libgc_version_minor "${libgc_version_minor}")
      set(LIBGC_VERSION_STRING "${libgc_version_major}.${libgc_version_minor}")
    endif()
    unset(libgc_version_major)
    unset(libgc_version_minor)
    unset(libgc_version_patch)
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibGc
    REQUIRED_VARS LIBGC_LIBRARIES LIBGC_INCLUDE_DIR
    VERSION_VAR LIBGC_VERSION_STRING)

mark_as_advanced(LIBGC_INCLUDE_DIR LIBGC_LIBRARIES)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLOGGING_ENABLED=1")

add_subdirectory(src)
if (ENABLE_UNITTESTS)
  add_subdirectory(test)
  include(CTest)
endif()
