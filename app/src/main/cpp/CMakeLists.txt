
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("aps3e")

add_compile_options(-march=armv8-a)
add_compile_options(-stdlib=libc++)
#add_compile_options(-flto=thin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_link_options(-Wl,-z,max-page-size=16384)

set(CMAKE_CXX_STANDARD 20)

project(aps3e LANGUAGES C CXX)

add_library(lang_System SHARED lang_System.cpp)

add_subdirectory(rpcs3 EXCLUDE_FROM_ALL)

add_library(emu SHARED)
set_target_properties(emu PROPERTIES OUTPUT_NAME "e")

target_link_options(emu PRIVATE -Wl,--exclude-libs,ALL)

#target_compile_options(emu PRIVATE -flto)
#target_link_options(emu PRIVATE -flto)

target_sources(emu
        PRIVATE
        rpcs3/rpcs3/Input/raw_mouse_config.cpp
        rpcs3/rpcs3/Input/raw_mouse_handler.cpp
        rpcs3/rpcs3/Input/product_info.cpp
        rpcs3/rpcs3/rpcs3_version.cpp
        rpcs3/rpcs3/stb_image.cpp
        rpcs3/rpcs3/stdafx.cpp
        aps3e.cpp
        vkapi.cpp
        vkutil.cpp
        cpuinfo.cpp
        glsl2spv.cpp
)

target_link_libraries(emu PRIVATE rpcs3_emu)
target_link_libraries(emu PRIVATE 3rdparty::yaml-cpp 3rdparty::hidapi 3rdparty::libusb 3rdparty::wolfssl 3rdparty::libcurl)
target_link_libraries(emu PRIVATE android)
