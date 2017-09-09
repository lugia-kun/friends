
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
set(LIBFRIENDS_LIBRARIES)
list(APPEND LIBFRIENDS_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND LIBFRIENDS_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}")
set(LIBFRIENDS_INCLUDE_DIR "${LIBFRIENDS_INCLUDES}" PARENT_SCOPE)
if (LIBEDIT_FOUND EQUAL 1)
  list(APPEND LIBFRIENDS_INCLUDES "${LIBEDIT_INCLUDE_DIR}")
  list(APPEND LIBFRIENDS_LIBRARIES "${LIBEDIT_LIBRARIES}")
endif()
set_target_properties(libfriends PROPERTIES
  INCLUDE_DIRECTORIES
  "${LIBFRIENDS_INCLUDES}"
  OUTPUT_NAME friends)
target_link_libraries(libfriends ${LIBFRIENDS_LIBRARIES})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/friends_config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/friends_config.h"
  )
