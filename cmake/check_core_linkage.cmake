execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${SOURCE}" -B "${BINARY}" -G Ninja
    "-DCMAKE_PREFIX_PATH=${SDK}"
    "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN}"
    "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}"
    "-DLINK_CASE=${LINK_CASE}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(result EQUAL 0)
    message(FATAL_ERROR "Mixed static Core / shared Application was accepted (${LINK_CASE})")
endif()
if(NOT "${output}${error}" MATCHES "INTERFACE_GUI_FORMS_CORE_LINKAGE|ERROR_do_not_link_static_Core_with_Application")
    message(FATAL_ERROR "Configure failed for an unrelated reason: ${output}${error}")
endif()
message(STATUS "Rejected mixed Core ownership through ${LINK_CASE}")
