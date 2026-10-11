if(NOT DEFINED GALLERY_PAGE_FILES)
    message(FATAL_ERROR "GALLERY_PAGE_FILES is required")
endif()

string(REPLACE "," ";" gallery_page_files "${GALLERY_PAGE_FILES}")

# The Gallery's page builders are a public-library consumer. Any local subclass
# can hide layout, paint, input, scheduler, or semantic behavior that should
# instead be implemented and tested in GUI.Forms itself. (The compiled-DML
# GalleryControl family lives in gallery_controls.cpp, outside this policy.)
foreach(gallery_page_file IN LISTS gallery_page_files)
    file(READ "${gallery_page_file}" gallery_page_source)
    if(gallery_page_source MATCHES
       "(^|[\r\n])[ \t]*(class|struct)[ \t]+[A-Za-z_][A-Za-z0-9_]*([ \t]+final)?[ \t\r\n]*:")
        message(FATAL_ERROR
            "Gallery pages contain a local subclass in ${gallery_page_file}; promote it to GUI.Forms")
    endif()
endforeach()

message(STATUS "Gallery pages use public GUI.Forms controls only")
