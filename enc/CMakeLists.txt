
add_re2c_target(mktble.c mktble.re)
add_executable(mktble mktble.c)
set_target_properties(mktble PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR})

function(add_mktble_target UCM TPL RTPL HTPL OUT ROUT NAME HNAME IANAME)
  add_custom_command(OUTPUT "${HNAME}"
    COMMAND "$<TARGET_FILE:mktble>" "${UCM}" "${HTPL}" "${HNAME}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${HTPL}" "$<TARGET_FILE:mktble>"
    COMMENT "[MKTBLE] Generating Header ${HNAME} for ${NAME}")
  add_custom_command(OUTPUT "${OUT}"
    COMMAND "$<TARGET_FILE:mktble>" "${UCM}" "${TPL}" "${OUT}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${TPL}" "$<TARGET_FILE:mktble>" "${HNAME}"
    COMMENT "[MKTBLE] Generating Forward Converter ${OUT} for ${NAME}")
  add_custom_command(OUTPUT "${ROUT}"
    COMMAND "$<TARGET_FILE:mktble>" "${UCM}" "${RTPL}" "${ROUT}" "${NAME}"
            "${HNAME}" "${IANAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${UCM}" "${RTPL}" "$<TARGET_FILE:mktble>" "${HNAME}"
    COMMENT "[MKTBLE] Generating Reverse Converter ${ROUT} for ${NAME}")
endfunction()
