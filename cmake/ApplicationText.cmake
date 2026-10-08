# Development text implementations use static Core for provider-only tests.
# Native consumers use the same implementation inside Application, never a
# second Core archive. Keep the old source-target names as consumer interfaces.
function(gui_forms_application_text consumer implementation public_name)
    if(NOT TARGET ${implementation})
        return()
    endif()
    add_library(${consumer} INTERFACE)
    add_library(GUIForms::${public_name} ALIAS ${consumer})
    if(NOT TARGET gui_forms_application)
        target_link_libraries(${consumer} INTERFACE ${implementation})
        set_property(TARGET ${consumer} PROPERTY INTERFACE_GUI_FORMS_CORE_LINKAGE static)
        set_property(TARGET ${consumer} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING GUI_FORMS_CORE_LINKAGE)
        return()
    endif()

    # Include every public entry point, even when the host itself never calls it.
    # The override also covers an ordinary private reference from a host/renderer.
    set_property(TARGET gui_forms_application PROPERTY
        LINK_LIBRARY_OVERRIDE_${implementation} WHOLE_ARCHIVE)
    target_link_libraries(gui_forms_application PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,${implementation}>")
    target_link_libraries(${consumer} INTERFACE GUIForms::Application)
    set_property(TARGET ${consumer} PROPERTY INTERFACE_GUI_FORMS_CORE_LINKAGE application)
    set_property(TARGET ${consumer} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING GUI_FORMS_CORE_LINKAGE)
endfunction()

gui_forms_application_text(gui_forms_text_masks gui_forms_text_masks_impl TextMasks)
gui_forms_application_text(gui_forms_prepared_text gui_forms_prepared_text_impl PreparedText)

if(TARGET gui_forms_application AND GUI_FORMS_BUILD_PREPARED_TEXT)
    target_compile_definitions(gui_forms_application PUBLIC
        $<BUILD_INTERFACE:GUI_FORMS_PREPARED_TEXT>)
endif()

if(GUI_FORMS_BUILD_TESTS AND TARGET GUIForms::Application AND
   TARGET GUIForms::TextMasks AND GUI_FORMS_BUILD_AUDIO_LOOP_TRANSPORT)
    add_subdirectory(tests/application_text_consumer)
    # Reproduce the source consumer graph in a fresh CMake generation, retaining
    # the parent's compiler, pinned sources and platform renderer configuration.
    set(application_text_cache "${CMAKE_CURRENT_BINARY_DIR}/application-text-cache.cmake")
    file(WRITE "${application_text_cache}" "# Generated consumer configuration.\n")
    get_cmake_property(application_text_variables CACHE_VARIABLES)
    foreach(variable IN LISTS application_text_variables)
        if(variable MATCHES "^(GUI_FORMS_|FETCHCONTENT_SOURCE_DIR_|CMAKE_TOOLCHAIN_FILE$|CMAKE_MAKE_PROGRAM$|CMAKE_BUILD_TYPE$)")
            file(APPEND "${application_text_cache}"
                "set(${variable} [==[${${variable}}]==] CACHE STRING \"Consumer configuration\" FORCE)\n")
        endif()
    endforeach()
    foreach(link_case IN ITEMS accepted static_core)
        add_test(NAME gui_forms_application_text_configure_${link_case}
            COMMAND "${CMAKE_COMMAND}"
                "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/tests/application_text_consumer"
                "-DPROVIDER=${CMAKE_CURRENT_SOURCE_DIR}"
                "-DBINARY=${CMAKE_CURRENT_BINARY_DIR}/application-text-${link_case}"
                "-DCACHE=${application_text_cache}"
                "-DLINK_CASE=${link_case}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_application_text.cmake")
        set_tests_properties(gui_forms_application_text_configure_${link_case}
            PROPERTIES TIMEOUT 180)
    endforeach()
endif()
