cmake_minimum_required(VERSION 3.24)
project(stx_cng_installed_consumer LANGUAGES CXX)

# This fixture has no source-tree include path or library fallback; the
# package config finds stx_zlib by itself, and the export carries the private
# Zstandard archive.
find_package(stx_cng 1.0 CONFIG REQUIRED)
add_executable(stx_cng_installed_consumer main.cpp)
target_compile_features(stx_cng_installed_consumer PRIVATE cxx_std_20)
target_link_libraries(stx_cng_installed_consumer PRIVATE stx_cng::stx_cng)
enable_testing()
add_test(NAME stx_cng_installed_consumer COMMAND stx_cng_installed_consumer)
