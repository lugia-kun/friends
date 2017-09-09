cmake_minimum_required(VERSION 3.0.2)

project(friends-lang C)

include(CMakeDependentOption)
include(CheckIncludeFile)
include(CheckPrototypeDefinition)
include(CheckSymbolExists)
include(CheckTypeSize)

CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
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

if("${USHORT_T}" EQUAL 2)
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

# Currently these apps are mandated. If we provide a source code for
# a *specific* configuration, these apps will not be mandatory (but
# still required for development).

find_package(RE2C "0.16" REQUIRED)
find_package(LEMON REQUIRED)

if(FRIENDS_USE_UTF8_INTERNAL)
  set(RE2C_ENCODING_FLAGS "-8")
else()
  set(RE2C_ENCODING_FLAGS "-x")
endif()

add_subdirectory(lib)
add_subdirectory(fe)

if (FRIENDS_ENABLE_TEST)
  enable_testing()
  add_subdirectory(tests)
endif()
