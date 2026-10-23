# Build-time generation of the font substitution records:
#   cmake -DTABLE=<json> -DFONT_DIRECTORY=<dir> -DOUTPUT=<inc> -P generate_font_substitution.cmake
include("${CMAKE_CURRENT_LIST_DIR}/FontSubstitution.cmake")
gui_forms_font_substitution_read(JSON "${TABLE}" FONT_DIRECTORY "${FONT_DIRECTORY}"
    FILES unused_files LICENCES unused_licences OUTPUT "${OUTPUT}")
