# Area labels for every ctest test (owner plan of 2026-10-22: cut redundant
# verification). tools/test_areas.json lists the areas in order; a test gets
# the label area_<area> of the first entry whose pattern its name matches, so
# `ctest -L area_gui` runs the Gallery tests and tools/change_selection.py can
# run only the areas a branch's changes reach. The JSON is the one source of
# both the labels and the path rules; editing it reconfigures the tree.
#
# Call gui_forms_label_test_areas() once, after the last add_test of the
# directory that calls it and of the subdirectories it names. A subdirectory's
# tests are labelled through the subdirectory's LABELS property, since a test
# property can be set only from the test's own directory before CMake 3.28;
# all of its tests must then fall in one area.

set(GUI_FORMS_TEST_AREAS_FILE "${CMAKE_CURRENT_LIST_DIR}/../tools/test_areas.json")

# Sets out_area to the area of test_name, or to the empty string when no
# pattern matches (the last pattern of the checked-in file matches every name).
function(gui_forms_test_area out_area test_name areas_json)
    string(JSON area_count LENGTH "${areas_json}")
    math(EXPR last_area "${area_count} - 1")
    foreach(area_index RANGE ${last_area})
        string(JSON area_name GET "${areas_json}" ${area_index} area)
        string(JSON area_pattern GET "${areas_json}" ${area_index} pattern)
        if(test_name MATCHES "${area_pattern}")
            set(${out_area} "${area_name}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${out_area} "" PARENT_SCOPE)
endfunction()

# Labels the tests of the calling directory, and of each directory in
# ARGN (subdirectories already added), with their areas.
function(gui_forms_label_test_areas)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GUI_FORMS_TEST_AREAS_FILE}")
    file(READ "${GUI_FORMS_TEST_AREAS_FILE}" areas_text)
    string(JSON areas_json GET "${areas_text}" test_areas)
    get_property(directory_tests DIRECTORY PROPERTY TESTS)
    foreach(test_name IN LISTS directory_tests)
        gui_forms_test_area(test_area "${test_name}" "${areas_json}")
        if(test_area)
            set_property(TEST "${test_name}" APPEND PROPERTY LABELS "area_${test_area}")
        endif()
    endforeach()
    foreach(subdirectory IN LISTS ARGN)
        get_property(subdirectory_tests DIRECTORY "${subdirectory}" PROPERTY TESTS)
        set(subdirectory_area "")
        foreach(test_name IN LISTS subdirectory_tests)
            gui_forms_test_area(test_area "${test_name}" "${areas_json}")
            if(subdirectory_area AND NOT test_area STREQUAL subdirectory_area)
                message(FATAL_ERROR "The tests of ${subdirectory} fall in the areas ${subdirectory_area} and "
                                    "${test_area} (${test_name}); its LABELS property can carry only one")
            endif()
            set(subdirectory_area "${test_area}")
        endforeach()
        if(subdirectory_area)
            set_property(DIRECTORY "${subdirectory}" APPEND PROPERTY LABELS "area_${subdirectory_area}")
        endif()
    endforeach()
endfunction()
