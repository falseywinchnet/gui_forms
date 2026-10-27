# An installed-package consumer of the components tools/build_sdk.py
# --features all adds: tools/verify_install.py --feature-components builds and
# runs it against a prefix. It compiles application_text_consumer's main.cpp
# (GUI_FORMS_CONSUMER_MAIN), which uses the text masks and the loop transport
# beside Application.
cmake_minimum_required(VERSION 3.25)
project(GUIFormsInstalledFeatureConsumer LANGUAGES CXX)
find_package(GUIForms 0.2 CONFIG REQUIRED COMPONENTS Application TextMasks Audio AudioLoopTransport)
get_target_property(text_core GUIForms::TextMasks INTERFACE_GUI_FORMS_CORE_LINKAGE)
if(NOT text_core STREQUAL "application")
    message(FATAL_ERROR "The installed TextMasks must advertise Application's Core, not '${text_core}'")
endif()
if(NOT EXISTS "${GUI_FORMS_CONSUMER_MAIN}")
    message(FATAL_ERROR "Set GUI_FORMS_CONSUMER_MAIN to tests/application_text_consumer/main.cpp")
endif()
add_executable(installed_feature_consumer "${GUI_FORMS_CONSUMER_MAIN}")
target_compile_features(installed_feature_consumer PRIVATE cxx_std_20)
target_link_libraries(installed_feature_consumer PRIVATE
    GUIForms::Application GUIForms::TextMasks GUIForms::AudioLoopTransport)
if(WIN32)
    add_custom_command(TARGET installed_feature_consumer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:GUIForms::Application>" "$<TARGET_FILE:GUIForms::Threading>"
            "$<TARGET_FILE_DIR:installed_feature_consumer>"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${GUIForms_FONT_DIR}" "$<TARGET_FILE_DIR:installed_feature_consumer>/fonts")
endif()
enable_testing()
add_test(NAME installed_feature_consumer COMMAND installed_feature_consumer)
