# Runs a test program in a mode that writes into read-only pages and passes only if the
# platform's memory-protection fault stopped it: SIGSEGV or SIGBUS on Linux and macOS
# (execute_process reports "Segmentation fault", "Bus error", "SIGSEGV" or "SIGBUS"), an access
# violation on Windows ("Access violation"). A normal exit with any status, an abort, a trap or a
# sanitizer report fails, so the mode cannot pass by ending for another reason.
#
# AddressSanitizer catches SIGSEGV and SIGBUS itself, reports and exits with status 1; the
# options appended here leave those signals to the platform, so the sanitizer tree checks the
# same fault as the ordinary one.
#
# Usage: cmake -DPROGRAM=<path> -DMODE=<--argument> -P expect_fault.cmake
if(NOT PROGRAM OR NOT MODE)
    message(FATAL_ERROR "expect_fault.cmake: PROGRAM and MODE are required")
endif()
if("$ENV{ASAN_OPTIONS}" STREQUAL "")
    set(ENV{ASAN_OPTIONS} "handle_segv=0:handle_sigbus=0")
else()
    set(ENV{ASAN_OPTIONS} "$ENV{ASAN_OPTIONS}:handle_segv=0:handle_sigbus=0")
endif()
execute_process(COMMAND "${PROGRAM}" "${MODE}" RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET)
if(result MATCHES "^-?[0-9]+$")
    message(FATAL_ERROR "${PROGRAM} ${MODE} exited with status ${result}; expected a protection fault")
endif()
if(NOT result MATCHES "^(Segmentation fault|SIGSEGV|Bus error|SIGBUS|Access violation)$")
    message(FATAL_ERROR "${PROGRAM} ${MODE} ended with \"${result}\"; expected a protection fault")
endif()
message(STATUS "${PROGRAM} ${MODE} faulted: ${result}")
