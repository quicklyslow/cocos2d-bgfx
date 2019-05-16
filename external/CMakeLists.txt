cmake_minimum_required(VERSION 3.6)
project(external)

add_library(external empty.cpp)

add_subdirectory(sources/lzma)
add_subdirectory(sources/tinyxml2)
add_subdirectory(sources/xxhash)
add_subdirectory(sources/xxtea)
add_subdirectory(sources/clipper)
add_subdirectory(sources/edtaa3func)
add_subdirectory(sources/ConvertUTF)
add_subdirectory(sources/poly2tri)
add_subdirectory(libs/freetype)
#add_subdirectory(libs/jpeg)
add_subdirectory(libs/openssl)
#add_subdirectory(libs/png)
#add_subdirectory(libs/tiff)
add_subdirectory(libs/uv)
add_subdirectory(libs/webp)
add_subdirectory(libs/websockets)

target_link_libraries(external
    ext_freetype
    #ext_jpeg
    ext_uv
    ext_webp
    ext_ssl
    ext_crypto
    ext_websockets
    ext_tinyxml2
    ext_xxhash
    ext_xxtea
    ext_clipper
    ext_edtaa3func
    ext_convertUTF
    ext_poly2tri
)

# use lua/js specific libs by property to prevent conflict
if(BUILD_JS_LIBS)
    if(ANDROID OR WINDOWS)
        add_subdirectory(libs/v8)
        set_property(TARGET external APPEND PROPERTY
            CC_JS_DEPEND ext_v8
        )
    endif()
endif()

if(NOT LINUX)
    add_subdirectory(libs/curl)
    add_subdirectory(libs/png)
    target_link_libraries(external 
        ext_curl
        ext_png
    )
endif(NOT LINUX)

if(ANDROID)
    add_subdirectory(sources/cpufeatures)
    add_subdirectory(sources/pvmp3dec)
    add_subdirectory(sources/tremolo)
    target_link_libraries(external 
        ext_cpufeatures 
        ext_pvmp3dec 
        ext_tremolo
    )
endif()

if(WINDOWS)
    # sqlite3 prebuilt only existed at windows
    add_subdirectory(sqlite3)
    add_subdirectory(win32-specific/gles)
    add_subdirectory(win32-specific/icon)
    add_subdirectory(win32-specific/MP3Decoder)
    add_subdirectory(win32-specific/OggDecoder)
    add_subdirectory(win32-specific/OpenalSoft)
    target_link_libraries(external 
        ext_sqlite3
        ext_gles
        ext_icon
        ext_MP3Decoder
        ext_OggDecoder
        ext_OpenalSoft
    )
endif()

if(WINDOWS OR MACOSX)
    add_subdirectory(glfw3)
    target_link_libraries(external 
        ext_glfw3
    )
endif()

if(MACOSX OR ANDROID OR WINDOWS)
    add_subdirectory(libs/zlib)
    target_link_libraries(external 
        ext_zlib
    )
endif()

# unzip depend on zlib
add_subdirectory(sources/unzip)
target_link_libraries(external ext_unzip)

# put "external" into External folder, too
set_target_properties(external
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

