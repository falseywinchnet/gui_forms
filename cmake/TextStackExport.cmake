# The pinned FreeType and HarfBuzz that GUI.Forms builds, installed for the
# products that call them directly (Pocket Library's page compositor includes
# ft2build.h and hb.h). GUI_FORMS_INSTALL_TEXT_STACK is off by default and only
# `tools/build_sdk.py --features all` turns it on, so the default SDK keeps
# them private to the text engine.
#
# Installed form: the two static libraries GUI.Forms links (both compiled with
# GUI.Forms' options, FreeType without zlib: cmake/freetype/gui_forms_ftoption.h),
# the upstream header layouts (include/freetype2 and include/harfbuzz, each the
# directory the target's INSTALL_INTERFACE names), and one export file,
# GUIFormsTextStackTargets.cmake, holding GUIForms::FreeType and
# GUIForms::HarfBuzz. GUIForms::HarfBuzz links GUIForms::FreeType. The
# licences are installed with the text engine's (CMakeLists.txt, HarfBuzz-COPYING.txt
# and FreeType-FTL.txt) and listed in THIRD_PARTY_NOTICES.md.
#
# The subprojects' own install rules stay unused: CMakeLists.txt adds both
# EXCLUDE_FROM_ALL with SKIP_INSTALL_ALL set, so their pkg-config files and
# freetype-config or harfbuzzConfig exports never exist, and freetype belongs
# to one export set only (CMake refuses a dependency exported twice).

if(GUI_FORMS_INSTALL_TEXT_STACK)
    if(NOT TARGET freetype OR NOT TARGET harfbuzz)
        message(FATAL_ERROR "GUI_FORMS_INSTALL_TEXT_STACK requires GUI_FORMS_ENABLE_HARFBUZZ_TEXT")
    endif()

    # FreeType: the source include tree without its internal headers, then the
    # configured ftconfig.h and ftoption.h the build generated (the text
    # engine compiles against those two as well).
    get_target_property(_gui_forms_freetype_binary_dir freetype BINARY_DIR)
    install(DIRECTORY "${GUI_FORMS_FREETYPE_ROOT}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/freetype2
        PATTERN "internal" EXCLUDE
        PATTERN "ftconfig.h" EXCLUDE
        PATTERN "ftoption.h" EXCLUDE)
    install(FILES
        "${_gui_forms_freetype_binary_dir}/include/freetype/config/ftconfig.h"
        "${_gui_forms_freetype_binary_dir}/include/freetype/config/ftoption.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/freetype2/freetype/config)
    unset(_gui_forms_freetype_binary_dir)

    # HarfBuzz: the public headers are the .h files among the target's
    # sources (its private headers are .hh), plus hb-cplusplus.hh and hb-ft.h
    # (listed upstream only for a system FreeType). Upstream's generated
    # hb-features.h is not installed: its install rules are skipped
    # (SKIP_INSTALL_ALL in CMakeLists.txt) and nothing includes it.
    get_target_property(_gui_forms_harfbuzz_sources harfbuzz SOURCES)
    set(_gui_forms_harfbuzz_headers
        "${GUI_FORMS_HARFBUZZ_ROOT}/src/hb-cplusplus.hh"
        "${GUI_FORMS_HARFBUZZ_ROOT}/src/hb-ft.h")
    foreach(_gui_forms_harfbuzz_source IN LISTS _gui_forms_harfbuzz_sources)
        if(_gui_forms_harfbuzz_source MATCHES "/src/hb[^/]*\\.h$")
            list(APPEND _gui_forms_harfbuzz_headers "${_gui_forms_harfbuzz_source}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES _gui_forms_harfbuzz_headers)
    install(FILES ${_gui_forms_harfbuzz_headers}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/harfbuzz)
    unset(_gui_forms_harfbuzz_source)
    unset(_gui_forms_harfbuzz_sources)
    unset(_gui_forms_harfbuzz_headers)

    set_target_properties(freetype PROPERTIES EXPORT_NAME FreeType)
    set_target_properties(harfbuzz PROPERTIES EXPORT_NAME HarfBuzz)
    install(TARGETS freetype harfbuzz
        EXPORT GUIFormsTextStackTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(EXPORT GUIFormsTextStackTargets
        FILE GUIFormsTextStackTargets.cmake
        NAMESPACE GUIForms::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GUIForms)
endif()
