
include(FindPackageHandleStandardArgs)

# Version info is not available (by executing it)
find_program(LEMON_EXECUTABLE lemon)

find_package_handle_standard_args(LEMON
  REQUIRED_VARS LEMON_EXECUTABLE)

if (LEMON_FOUND)
  set(LEMONFLAGS "" CACHE STRING "lemon flags")
  mark_as_advanced(LEMONFLAGS)
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
endif()
