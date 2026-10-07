cmake_minimum_required(VERSION 3.20)

project(
    HideHomeGallery
    VERSION 1.0.0
    LANGUAGES CXX RC
)

if(NOT WIN32)
    message(
        FATAL_ERROR
        "HideHomeGallery 只能在 Windows 上构建。"
    )
endif()

add_executable(
    HideHomeGallery
    WIN32

    src/main.cpp
    src/Window.cpp
    src/Registry.cpp

    resources/app.rc
)

set_target_properties(
    HideHomeGallery
    PROPERTIES
    OUTPUT_NAME "HideHomeGallery"
    # 让 VS 使用我们的 manifest，避免与默认 manifest 冲突
    VS_GLOBAL_ApplicationManifest "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.manifest"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_features(
    HideHomeGallery
    PRIVATE
    cxx_std_17
)

target_include_directories(
    HideHomeGallery
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(
    HideHomeGallery
    PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(
    HideHomeGallery
    PRIVATE
    user32
    advapi32
    shell32
    comctl32
    gdi32
)

if(MSVC)
    target_compile_options(
        HideHomeGallery
        PRIVATE
        /W4
        /utf-8
    )

    target_link_options(
        HideHomeGallery
        PRIVATE
        /SUBSYSTEM:WINDOWS
        # 确保嵌入 manifest
        /MANIFEST:EMBED
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        HideHomeGallery
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_link_options(
        HideHomeGallery
        PRIVATE
        -municode
        -mwindows
    )
endif()
