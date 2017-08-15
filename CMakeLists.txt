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

find_program(RE2C_EXECUTABLE re2c
  HINTS /bin /usr/bin /usr/local/bin)
set(RE2CFLAGS "-s" CACHE STRING "re2c flags")
set(RE2CFLAGS_DEBUG "-W" CACHE STRING "re2c debug-only flags")
set(RE2CFLAGS_RELEASE "--no-generation-date -i" CACHE STRING "re2c release-only flags")
set(RE2CFLAGS_MINSIZEREL "" CACHE STRING "re2c minsizerel flags")
set(RE2CFLAGS_RELWITHDEBINFO "" CACHE STRING "re2c relwithdebinfo flags")

find_program(LEMON_EXECUTABLE lemon)

function(add_re2c_target OUT IN)
  get_source_file_property(FS "${IN}" COMPILE_OPTIONS)
  if (FS)
    string(REPLACE " " ";" FS "${FS}")
  else()
    set(FS)
  endif()
  string(TOUPPER "${CMAKE_BUILD_TYPE}" U)
  string(REPLACE " " ";" F "${RE2CFLAGS}")
  string(REPLACE " " ";" FN "${RE2CFLAGS_${U}}")
  if(NOT IS_ABSOLUTE "${IN}")
    get_source_file_property(GEN "${CMAKE_CURRENT_BINARY_DIR}/${IN}"
      GENERATED)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${IN}" OR GEN)
      set(IN "${CMAKE_CURRENT_BINARY_DIR}/${IN}")
    else()
      set(IN "${CMAKE_CURRENT_SOURCE_DIR}/${IN}")
    endif()
  endif()
  add_custom_command(OUTPUT "${OUT}"
    COMMAND "${RE2C_EXECUTABLE}" ${F} ${FN} ${FS}
    -o "${OUT}" "${IN}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${IN}"
    COMMENT "[RE2C] Generating ${OUT}")
endfunction()

set(LEMONFLAGS "" CACHE STRING "lemon flags")
function(add_lemon_target OUT IN)
  string(TOUPPER "${CMAKE_BUILD_TYPE}" U)
  string(REPLACE " " ";" F "${LEMONFLAGS}")
  if(NOT IS_ABSOLUTE "${IN}")
    get_source_file_property(GEN "${CMAKE_CURRENT_BINARY_DIR}/${IN}"
      GENERATED)
    if(GEN)
      set(IN "${CMAKE_CURRENT_BINARY_DIR}/${IN}")
    else()
      set(IN "${CMAKE_CURRENT_SOURCE_DIR}/${IN}")
    endif()
  endif()
  string(REGEX REPLACE ".c$" ".lem" OUT_BASE "${OUT}")
  string(REGEX REPLACE ".c$" ".h" OUT_HEADER "${OUT}")
  add_custom_command(OUTPUT "${OUT}" "${OUT_HEADER}"
    COMMAND "${CMAKE_COMMAND}" "-E" "copy" "${IN}" "${OUT_BASE}"
    COMMAND "${LEMON_EXECUTABLE}" ${F} "${OUT_BASE}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${IN}"
    COMMENT "[LEMON] Generating ${OUT}")
endfunction()

if(FRIENDS_USE_UTF8_INTERNAL)
  set(RE2C_ENCODING_FLAGS "-8")
else()
  set(RE2C_ENCODING_FLAGS "-x")
endif()

add_subdirectory(enc)

set(FRIENDS_C_SOURCES
  friends_core.c
  friends_list.c friends_set.c
  friends_io.c friends_error.c
  friends_park.c friends_data.c
  friends_atom.c
  friends_global.c friends_test.c)
set(FRIENDS_LEMON_SOURCES friends_parser.lem)
set(FRIENDS_RE2C_SOURCES  friends_lexer.re friends_string.re
  friends_printf.re friends_pathname.re
  enc/utf8.re  enc/utf8r.re)

if(FRIENDS_ENABLE_SJIS)
  add_mktble_target(${CMAKE_CURRENT_SOURCE_DIR}/enc/windows-932-2000.ucm
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktble.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktbler.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktble.htpl
    enc/sjis.re enc/sjisr.re Sjis enc/sjis.h "Shift_JIS")
  list(APPEND FRIENDS_RE2C_SOURCES
    enc/sjis.re  enc/sjisr.re)
  set_source_files_properties(enc/sjisr.re PROPERTIES
    COMPILE_OPTIONS "")
endif()
if(FRIENDS_ENABLE_EUCJP)
  add_mktble_target(${CMAKE_CURRENT_SOURCE_DIR}/enc/glibc-EUC_JP-2.3.3.ucm
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktble.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktbler.tpl
    ${CMAKE_CURRENT_SOURCE_DIR}/enc/mktble.htpl
    enc/eucjp.re enc/eucjpr.re Eucjp enc/eucjp.h "EUC-JP")
  list(APPEND FRIENDS_RE2C_SOURCES
    enc/eucjp.re enc/eucjpr.re)
  set_source_files_properties(enc/eucjpr.re PROPERTIES
    COMPILE_OPTIONS "")
endif()

set_source_files_properties(${FRIENDS_RE2C_SOURCES} PROPERTIES
  COMPILE_OPTIONS "${RE2C_ENCODING_FLAGS}")

set_source_files_properties(enc/utf8r.re PROPERTIES
  COMPILE_OPTIONS "-8")

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

set_target_properties(libfriends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}"
  OUTPUT_NAME friends)
set(LIBFRIENDS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}")

add_executable(friends main.c)
target_link_libraries(friends libfriends)

set_target_properties(friends PROPERTIES
  INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/friends-config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/friends-config.h"
  )
