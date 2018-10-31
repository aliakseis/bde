cmake_minimum_required(VERSION 3.11)

set(CMAKE_SUPPRESS_REGENERATION TRUE)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

project(bde)

add_definitions(-DBDE_BUILD_TARGET_EXC)
add_definitions(-DBSLS_NOTHROW_SPEC=noexcept)

add_definitions(-DBSLS_PLATFORM_CMP_MSVC)
# MSVC 2015 or later
add_definitions(-DBSLS_PLATFORM_CMP_VERSION=1900)

add_definitions(-DPCRE2_CODE_UNIT_WIDTH=8)
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DSUPPORT_UNICODE)

add_definitions(-DWNT)
add_definitions(-DDECIMAL_CALL_BY_REFERENCE=0)
add_definitions(-DDECIMAL_GLOBAL_ROUNDING=1)
add_definitions(-DDECIMAL_GLOBAL_EXCEPTION_FLAGS=0)
add_definitions(-DBID_BIG_ENDIAN=0)

include( ${CMAKE_SOURCE_DIR}/ModuleDirs.cmake )
