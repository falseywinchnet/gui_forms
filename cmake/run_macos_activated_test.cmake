# Runs a macOS test bundle through LaunchServices (`open`) so that it is an
# activated application whose window can become key. A process started
# directly from a shell outside the console session (ssh, some CI agents)
# cannot activate and receives no keyboard focus.
#
# `open -W` waits for the program but does not return its exit status, and it
# does not pass the caller's environment on. The sanitizer options are
# forwarded with --env, and the run passes only when the program's last
# output line is "failures=0" and its standard error holds no sanitizer
# report.
#
# Inputs: BUNDLE, the .app directory; OUTPUT_DIRECTORY, where the program's
# standard output and error are written.

if(NOT IS_DIRECTORY "${BUNDLE}")
    message(FATAL_ERROR "Test bundle is missing: ${BUNDLE}")
endif()
get_filename_component(bundle_name "${BUNDLE}" NAME_WE)
set(stdout_file "${OUTPUT_DIRECTORY}/${bundle_name}.stdout.txt")
set(stderr_file "${OUTPUT_DIRECTORY}/${bundle_name}.stderr.txt")
file(REMOVE "${stdout_file}" "${stderr_file}")

set(open_arguments -W -n --stdout "${stdout_file}" --stderr "${stderr_file}")
foreach(variable IN ITEMS ASAN_OPTIONS UBSAN_OPTIONS LSAN_OPTIONS)
    if(DEFINED ENV{${variable}})
        list(APPEND open_arguments --env "${variable}=$ENV{${variable}}")
    endif()
endforeach()

execute_process(
    COMMAND open ${open_arguments} "${BUNDLE}"
    RESULT_VARIABLE open_result
    TIMEOUT 120)

set(program_output "")
set(program_errors "")
if(EXISTS "${stdout_file}")
    file(READ "${stdout_file}" program_output)
endif()
if(EXISTS "${stderr_file}")
    file(READ "${stderr_file}" program_errors)
endif()
message("${program_output}")
if(NOT program_errors STREQUAL "")
    message("${program_errors}")
endif()

if(NOT open_result EQUAL 0)
    message(FATAL_ERROR "open failed for ${BUNDLE}: ${open_result}")
endif()
if(program_errors MATCHES "Sanitizer|runtime error")
    message(FATAL_ERROR "${bundle_name} reported a sanitizer finding")
endif()
if(NOT program_output MATCHES "\nfailures=0\n$")
    message(FATAL_ERROR "${bundle_name} did not finish with failures=0")
endif()
