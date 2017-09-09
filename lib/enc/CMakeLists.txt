
if(NOT EXISTS "${UCM2RE_EXECUTABLE}")
  add_re2c_target(ucm2re.c ucm2re.re)
  add_executable(ucm2re ucm2re.c)
  set_target_properties(ucm2re PROPERTIES INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR})
  set(UCM2RE_EXECUTABLE "$<TARGET_FILE:ucm2re>")
endif()
set(UCM2RE_EXECUTABLE "${UCM2RE_EXECUTABLE}" CACHE STRING
  "Path to ucm2re executable")

function(add_ucm2re_target UCM TPL RTPL HTPL OUT ROUT NAME HNAME IANAME)
  add_custom_command(OUTPUT "${HNAME}"
    COMMAND "${UCM2RE_EXECUTABLE}" "${UCM}" "${HTPL}" "${HNAME}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${HTPL}" "${UCM2RE_EXECUTABLE}"
    COMMENT "[UCM2RE] Generating Header ${HNAME} for ${NAME}")
  add_custom_command(OUTPUT "${OUT}"
    COMMAND "${UCM2RE_EXECUTABLE}" "${UCM}" "${TPL}" "${OUT}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${TPL}" "${UCM2RE_EXECUTABLE}" "${HNAME}"
    COMMENT "[UCM2RE] Generating Forward Converter ${OUT} for ${NAME}")
  add_custom_command(OUTPUT "${ROUT}"
    COMMAND "${UCM2RE_EXECUTABLE}" "${UCM}" "${RTPL}" "${ROUT}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${RTPL}" "${UCM2RE_EXECUTABLE}" "${HNAME}"
    COMMENT "[UCM2RE] Generating Reverse Converter ${ROUT} for ${NAME}")
endfunction()
