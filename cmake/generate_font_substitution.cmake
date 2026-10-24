# Build-time generation of the font substitution records:
#   cmake -DTABLE=<json> -DFONT_DIRECTORY=<dir> -DOUTPUT=<inc> -P generate_font_substitution.cmake
# A -P script starts with no policies set; the generator needs the project's
# (CMP0057, if(IN_LIST)), which CMake 4 enables by default and CMake 3 does not.
cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/FontSubstitution.cmake")
gui_forms_font_substitution_read(JSON "${TABLE}" FONT_DIRECTORY "${FONT_DIRECTORY}"
    FILES unused_files LICENCES unused_licences OUTPUT "${OUTPUT}")
