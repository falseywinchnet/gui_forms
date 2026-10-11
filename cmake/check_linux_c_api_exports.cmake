# A C ABI shared library on Linux exports exactly one dynamic symbol, its
# entry point. Without a version script it also exported the C++ internals of
# every static library linked into it.
if(NOT DEFINED C_API_LIBRARY OR NOT DEFINED EXPECTED_SYMBOL)
    message(FATAL_ERROR "C_API_LIBRARY and EXPECTED_SYMBOL are required")
endif()

execute_process(
    COMMAND nm -D --defined-only "${C_API_LIBRARY}"
    RESULT_VARIABLE nm_result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE nm_error
)
if(NOT nm_result EQUAL 0)
    message(FATAL_ERROR "nm failed for ${C_API_LIBRARY}: ${nm_error}")
endif()

string(REGEX MATCHALL "[0-9a-fA-F]+ [A-Za-z] [^\n]+" entries "${output}")
set(exported_symbols)
foreach(entry IN LISTS entries)
    string(REGEX REPLACE "^[0-9a-fA-F]+ [A-Za-z] " "" name "${entry}")
    list(APPEND exported_symbols "${name}")
endforeach()
list(REMOVE_DUPLICATES exported_symbols)
list(LENGTH exported_symbols export_count)
if(NOT export_count EQUAL 1 OR NOT exported_symbols STREQUAL EXPECTED_SYMBOL)
    message(FATAL_ERROR
        "${C_API_LIBRARY} must export only ${EXPECTED_SYMBOL}; found ${export_count}: ${exported_symbols}")
endif()
message(STATUS "Linux C ABI export policy passed: ${EXPECTED_SYMBOL}")
