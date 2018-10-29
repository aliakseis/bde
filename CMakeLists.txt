cmake_minimum_required(VERSION 3.12)

set(CMAKE_SUPPRESS_REGENERATION TRUE)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

project(bde)

add_compile_definitions(BDE_BUILD_TARGET_EXC)
add_compile_definitions(BSLS_NOTHROW_SPEC=noexcept)

add_compile_definitions(BSLS_PLATFORM_CMP_MSVC)
# MSVC 2015 or later
add_compile_definitions(BSLS_PLATFORM_CMP_VERSION=1900)

add_compile_definitions(PCRE2_CODE_UNIT_WIDTH=8)
add_compile_definitions(HAVE_CONFIG_H)
add_compile_definitions(SUPPORT_UNICODE)

add_compile_definitions(WNT)

include( ${CMAKE_SOURCE_DIR}/ModuleDirs.cmake )
