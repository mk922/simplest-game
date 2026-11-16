cmake_minimum_required(VERSION 3.22.1)
project(simplestgame LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
)
add_library(${PROJECT_NAME} SHARED
        main.cpp
)
target_compile_options(${PROJECT_NAME} PRIVATE
        "-fno-exceptions"
        "-fno-rtti"
)
target_link_options(${PROJECT_NAME} PRIVATE
        "-Wl,-u,ANativeActivity_onCreate"
        "-Wl,-z,max-page-size=65536"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
        native_app_glue
        android
        EGL
        GLESv3
        log
)
