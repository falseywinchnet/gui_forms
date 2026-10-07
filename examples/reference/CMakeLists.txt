cmake_minimum_required(VERSION 3.25)
project(GUIFormsOwnedEventsReference LANGUAGES CXX)

find_package(GUIForms 0.1 CONFIG REQUIRED COMPONENTS Controls)
add_executable(owned_events owned_events.cpp)
target_compile_features(owned_events PRIVATE cxx_std_20)
target_link_libraries(owned_events PRIVATE GUIForms::Controls)
# Keep the selected SDK ahead of an older GUI.Forms installation in the
# compiler's normal include search path.
set_target_properties(owned_events PROPERTIES NO_SYSTEM_FROM_IMPORTED TRUE)

enable_testing()
add_test(NAME owned_events COMMAND owned_events)

foreach(example_name IN ITEMS bound_values shared_state collection_state)
    add_executable(${example_name} ${example_name}.cpp)
    target_compile_features(${example_name} PRIVATE cxx_std_20)
    target_link_libraries(${example_name} PRIVATE GUIForms::Controls)
    set_target_properties(${example_name} PROPERTIES NO_SYSTEM_FROM_IMPORTED TRUE)
    add_test(NAME ${example_name} COMMAND ${example_name})
endforeach()

if(TARGET GUIForms::Application)
    add_executable(collection_gallery collection_gallery.cpp)
    target_compile_features(collection_gallery PRIVATE cxx_std_20)
    target_link_libraries(collection_gallery PRIVATE GUIForms::Application)
    set_target_properties(collection_gallery PROPERTIES NO_SYSTEM_FROM_IMPORTED TRUE)
endif()
