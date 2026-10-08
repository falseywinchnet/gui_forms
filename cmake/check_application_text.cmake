execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${SOURCE}" -B "${BINARY}" -G Ninja -C "${CACHE}"
    "-DGUI_FORMS_SOURCE_DIR=${PROVIDER}" "-DLINK_CASE=${LINK_CASE}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
file(WRITE "${BINARY}/configure-result.log" "${output}${error}")
if(LINK_CASE STREQUAL "accepted")
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Application + TextMasks + Audio rejected: ${output}${error}")
    endif()
    return()
endif()
if(result EQUAL 0)
    message(FATAL_ERROR "Application + static Core was accepted")
endif()
if(NOT "${output}${error}" MATCHES "INTERFACE_GUI_FORMS_CORE_LINKAGE|ERROR_do_not_link_static_Core_with_Application")
    message(FATAL_ERROR "Configure failed for an unrelated reason: ${output}${error}")
endif()
