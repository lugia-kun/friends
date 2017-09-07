cmake_minimum_required(VERSION 2.8.12)

project(friends-lang C)

include(CMakeDependentOption)
include(CheckIncludeFile)
include(CheckPrototypeDefinition)
include(CheckFunctionExists)
include(CheckTypeSize)

CHECK_INCLUDE_FILE("wchar.h" HAVE_WCHAR_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
if(HAVE_WCHAR_H)
  set(CMAKE_EXTRA_INCLUDE_FILES "wchar.h")
endif()
CHECK_TYPE_SIZE("unsigned wchar_t" WCHAR_T)
CHECK_TYPE_SIZE("unsigned short"   USHORT_T)
CHECK_TYPE_SIZE("uint16_t"         UINT16_T)

if(${CMAKE_C_COMPILER_ID} MATCHES MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(LIBEDIT libedit)
else()
  unset(LIBEDIT_FOUND)
endif()

if("${WCHAR_T}" EQUAL 2)
  set(FRIENDS_CTYPE "WCHAR_T")
elseif("${USHORT_T}" EQUAL 2)
  set(FRIENDS_CTYPE "USHORT_T")
elseif("${UINT16_T}" EQUAL 2)
  set(FRIENDS_CTYPE "UINT16_T")
else()
  unset(FRIENDS_CTYPE)
endif()
if(NOT DEFINED FRIENDS_CTYPE)
  set(FRIENDS_USE_UTF8_INTERNAL ON)
else()
  option(FRIENDS_USE_UTF8_INTERNAL "Use UTF-8 for internal encoding" OFF)
endif()
option(FRIENDS_ENABLE_SJIS  "Enable character set SJIS" ON)
option(FRIENDS_ENABLE_EUCJP "Enable character set EUC-JP" ON)
option(FRIENDS_ENABLE_TEST  "Enable test" ON)
option(FRIENDS_ENABLE_FULLPATH_SOURCE_NAME
  "Print fullpath of source file on errors" OFF)
mark_as_advanced(FRIENDS_ENABLE_FULLPATH_SOURCE_NAME)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Currently these apps are mandatory. If we provide a source code for
# a *specific* configuration, these apps will not be mandatory (but
# still required for development).

find_package(RE2C "0.16" REQUIRED)
find_package(LEMON REQUIRED)

if(FRIENDS_USE_UTF8_INTERNAL)
  set(RE2C_ENCODING_FLAGS "-8")
else()
  set(RE2C_ENCODING_FLAGS "-x")
endif()

add_subdirectory(enc)

set(FRIENDS_C_SOURCES
  friends_core.c
  friends_list.c friends_set.c friends_io.c friends_error.c
  friends_park.c friends_data.c friends_variable.c
  friends_atom.c friends_proposition.c friends_match.c
  friends_token.c friends_parser_implement.c
  friends_parser.c friends_global.c)
set(FRIENDS_LEMON_SOURCES friends_grammer.lem)
set(FRIENDS_RE2C_SOURCES  friends_lexer.re friends_string.re
  friends_printf.re friends_pathname.re
  enc/utf8.re  enc/utf8r.re)

if(FRIENDS_ENABLE_SJIS)
  add_ucm2re_target(${CMAKE_CURRENT_SOURCE_DIR}/enc/windows-932-2000.ucm
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2re.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2rer.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2re.htpl
    enc/sjis.re enc/sjisr.re Sjis enc/sjis.h "Shift_JIS")
  list(APPEND FRIENDS_RE2C_SOURCES
    enc/sjis.re  enc/sjisr.re)
endif()
if(FRIENDS_ENABLE_EUCJP)
  add_ucm2re_target(${CMAKE_CURRENT_SOURCE_DIR}/enc/glibc-EUC_JP-2.3.3.ucm
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2re.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2rer.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/ucm2re.htpl
    enc/eucjp.re enc/eucjpr.re Eucjp enc/eucjp.h "EUC-JP")
  list(APPEND FRIENDS_RE2C_SOURCES
    enc/eucjp.re enc/eucjpr.re)
endif()

set_source_files_properties(${FRIENDS_RE2C_SOURCES} PROPERTIES
  COMPILE_OPTIONS "${RE2C_ENCODING_FLAGS}")

set_source_files_properties(enc/utf8r.re PROPERTIES
  COMPILE_OPTIONS "-8")

if (FRIENDS_ENABLE_SJIS)
  set_source_files_properties(enc/sjisr.re PROPERTIES
    COMPILE_OPTIONS "")
endif()

if (FRIENDS_ENABLE_EUCJP)
  set_source_files_properties(enc/eucjpr.re PROPERTIES
    COMPILE_OPTIONS "")
endif()

set(FRIENDS_RE2C_C_SOURCES)
foreach(__RESRC ${FRIENDS_RE2C_SOURCES})
  string(REGEX REPLACE "\\.[^.]*$" ".c" __CSRC "${__RESRC}")
  list(APPEND FRIENDS_RE2C_C_SOURCES "${__CSRC}")
  if(RE2C_EXECUTABLE)
    add_re2c_target("${__CSRC}" "${__RESRC}")
  endif()
endforeach()

set(FRIENDS_LEMON_C_SOURCES)
foreach(__LESRC ${FRIENDS_LEMON_SOURCES})
  string(REGEX REPLACE "\\.[^.]*$" ".c" __CSRC "${__LESRC}")
  list(APPEND FRIENDS_LEMON_C_SOURCES "${__CSRC}")
  if(LEMON_EXECUTABLE)
    add_lemon_target("${__CSRC}" "${__LESRC}")
  endif()
endforeach()

add_library(libfriends STATIC
  ${FRIENDS_C_SOURCES}
  ${FRIENDS_RE2C_C_SOURCES}
  ${FRIENDS_LEMON_C_SOURCES})

set(LIBFRIENDS_INCLUDES)
list(APPEND LIBFRIENDS_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND LIBFRIENDS_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}")
set(LIBFRIENDS_INCLUDE_DIR "${LIBFRIENDS_INCLUDES}")
if (LIBEDIT_FOUND EQUAL 1)
  list(APPEND LIBFRIENDS_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}")
endif()
set_target_properties(libfriends PROPERTIES
  INCLUDE_DIRECTORIES
  "${LIBFRIENDS_INCLUDES}"
  OUTPUT_NAME friends)

add_executable(friends main.c)
target_link_libraries(friends libfriends)

set_target_properties(friends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};${LIBFRIENDS_INCLUDE_DIR}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/friends_config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/friends_config.h"
  )

if (FRIENDS_ENABLE_TEST)
  enable_testing()
  add_subdirectory(tests)
endif()
