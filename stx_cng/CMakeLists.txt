cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(stx_cng VERSION 0.2.0 LANGUAGES C CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(STX_CNG_TESTS_DEFAULT ON)
else()
    set(STX_CNG_TESTS_DEFAULT ${GUI_FORMS_BUILD_TESTS})
endif()
option(STX_CNG_TESTS "Build deterministic codec tests" ${STX_CNG_TESTS_DEFAULT})
option(STX_CNG_SANITIZE "Instrument library and tests" OFF)

# CRC32 comes from the sibling stx_zlib package. Inside GUI.Forms its target
# already exists; a standalone build finds an installed stx_zlib.
if(NOT TARGET stx_zlib::stx_zlib)
    find_package(stx_zlib 0.1 CONFIG REQUIRED)
endif()

# Zstandard 1.5.7, built from its official release tarball, pinned by SHA-256.
# tools/fetch_zstd.sh downloads and verifies it; configuration verifies the
# archive again and extracts only lib/ and the licence into the build tree.
set(STX_CNG_ZSTD_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/zstd-1.5.7.tar.gz"
    CACHE FILEPATH "Zstandard 1.5.7 release tarball (tools/fetch_zstd.sh)")
set(STX_CNG_ZSTD_SHA256 eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3)
if(NOT EXISTS "${STX_CNG_ZSTD_ARCHIVE}")
    message(FATAL_ERROR "Zstandard 1.5.7 is absent. Run stx_cng/tools/fetch_zstd.sh")
endif()
file(SHA256 "${STX_CNG_ZSTD_ARCHIVE}" stx_cng_zstd_actual)
if(NOT stx_cng_zstd_actual STREQUAL STX_CNG_ZSTD_SHA256)
    message(FATAL_ERROR "Zstandard archive hash ${stx_cng_zstd_actual} is not the pinned ${STX_CNG_ZSTD_SHA256}")
endif()
set(STX_CNG_ZSTD_ROOT "${CMAKE_CURRENT_BINARY_DIR}/zstd/zstd-1.5.7")
if(NOT EXISTS "${STX_CNG_ZSTD_ROOT}/lib/zstd.h")
    file(ARCHIVE_EXTRACT INPUT "${STX_CNG_ZSTD_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/zstd"
        PATTERNS zstd-1.5.7/lib zstd-1.5.7/LICENSE)
endif()
# The checked-in licence copy (installed, and shipped by GUI.Forms) must be
# the release's own.
file(SHA256 "${STX_CNG_ZSTD_ROOT}/LICENSE" stx_cng_zstd_licence)
file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE-zstd.txt" stx_cng_zstd_licence_copy)
if(NOT stx_cng_zstd_licence STREQUAL stx_cng_zstd_licence_copy)
    message(FATAL_ERROR "stx_cng/LICENSE-zstd.txt differs from Zstandard 1.5.7's LICENSE")
endif()
include(cmake/zstd.cmake)

add_library(stx_cng STATIC src/codec.cpp src/sequences.cpp src/literals.cpp src/restore.cpp)
add_library(stx_cng::stx_cng ALIAS stx_cng)
target_compile_features(stx_cng PUBLIC cxx_std_20)
set_target_properties(stx_cng PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)
target_include_directories(stx_cng PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
# Zstandard stays private: no zstd type or header reaches the public API. The
# installed archive still needs it at link time, which the export records.
target_link_libraries(stx_cng PUBLIC stx_zlib::stx_zlib PRIVATE stx_cng_zstd)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(stx_cng PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(MSVC)
    target_compile_options(stx_cng PRIVATE /W4)
endif()
if(STX_CNG_SANITIZE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "This sanitizer configuration needs Clang or GCC")
    endif()
    # Every undefined-behaviour report stops the test, as in the repository's
    # sanitizer build (tools/ci_build.py). Zstandard is instrumented too.
    set(stx_cng_sanitize -fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
    target_compile_options(stx_cng PRIVATE ${stx_cng_sanitize})
    target_compile_options(stx_cng_zstd PRIVATE ${stx_cng_sanitize})
    target_link_options(stx_cng INTERFACE -fsanitize=address,undefined)
endif()

if(STX_CNG_TESTS)
    enable_testing()
    foreach(test core malformed limits)
        add_executable(stx_cng_${test} tests/${test}.cpp)
        target_link_libraries(stx_cng_${test} PRIVATE stx_cng::stx_cng)
        if(STX_CNG_SANITIZE)
            target_compile_options(stx_cng_${test} PRIVATE ${stx_cng_sanitize})
        endif()
        add_test(NAME stx_cng_${test} COMMAND stx_cng_${test})
    endforeach()
    # The malformed-input test builds Zstandard frames of its own.
    target_link_libraries(stx_cng_malformed PRIVATE stx_cng_zstd)
    add_executable(stx_cng_fuzz_harness fuzz/cng_fuzz.cpp)
    target_link_libraries(stx_cng_fuzz_harness PRIVATE stx_cng::stx_cng)
    target_compile_definitions(stx_cng_fuzz_harness PRIVATE STX_CNG_FUZZ_STANDALONE=1)
    if(STX_CNG_SANITIZE)
        target_compile_options(stx_cng_fuzz_harness PRIVATE ${stx_cng_sanitize})
    endif()
    add_test(NAME stx_cng_fuzz_harness COMMAND stx_cng_fuzz_harness)
endif()

install(TARGETS stx_cng stx_cng_zstd EXPORT stx_cngTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT stx_cngTargets NAMESPACE stx_cng:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stx_cng)
configure_package_config_file(cmake/stx_cngConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/stx_cngConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stx_cng)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/stx_cngConfigVersion.cmake
    VERSION 0.2.0 COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stx_cngConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/stx_cngConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stx_cng)
install(FILES LICENSE PROVENANCE.md DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/stx_cng)
install(FILES LICENSE-zstd.txt DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/stx_cng RENAME zstd-LICENSE.txt)
