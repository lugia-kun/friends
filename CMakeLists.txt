cmake_minimum_required(VERSION 3.1.3)

project(friends-lang C)

include(CMakeDependentOption)
include(CheckIncludeFile)
include(CheckPrototypeDefinition)
include(CheckSymbolExists)
include(CheckTypeSize)

set(CMAKE_C_STANDARD 11)

CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_TYPE_SIZE("unsigned short" USHORT_T)
CHECK_TYPE_SIZE("uint16_t" UINT16_T)
CHECK_TYPE_SIZE("uint32_t" UINT32_T)
CHECK_TYPE_SIZE("max_align_t" MAX_ALIGN_T)

if(${CMAKE_C_COMPILER_ID} MATCHES MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(LIBEDIT libedit)
else()
  unset(LIBEDIT_FOUND)
endif()
CMAKE_DEPENDENT_OPTION(FRIENDS_ENABLE_LIBEDIT "Use libedit" ON
  "LIBEDIT_FOUND" OFF)

CMAKE_DEPENDENT_OPTION(FRIENDS_USE_UTF8_INTERNAL
  "Use UTF-8 for internal encoding" OFF
  "NOT HAVE_UTF16_T" ON)
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
