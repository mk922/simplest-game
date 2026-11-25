cmake_minimum_required(VERSION 3.22.1)
project(simplestgame)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
add_library(native_app_glue STATIC
        ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
target_include_directories(native_app_glue PUBLIC
        ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue
)
add_library(${CMAKE_PROJECT_NAME} SHARED
        main.cpp
)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -Wl,-z,max-page-size=65536
        -Wl,-u,ANativeActivity_onCreate
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        native_app_glue
        android
        EGL
        GLESv3
        log
)
