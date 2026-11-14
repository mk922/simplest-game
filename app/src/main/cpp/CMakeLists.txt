cmake_minimum_required(VERSION 3.22.1)
project(simplestgame LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
add_library(${PROJECT_NAME} SHARED
        main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
)
target_compile_options(${PROJECT_NAME} PRIVATE
        "-fno-exceptions"
        "-fno-rtti"
        "-fvisibility=hidden"
)
target_link_options(${PROJECT_NAME} PRIVATE
        "-Wl,-z,max-page-size=65536"
        "-Wl,--Bsymbolic-functions"
        "-Wl,-u,ANativeActivity_onCreate"
)
target_link_libraries(${PROJECT_NAME}
        android
        EGL
        GLESv3
        log
        native_app_glue
)
