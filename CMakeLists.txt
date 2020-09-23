cmake_minimum_required(VERSION 3.16)

project(Usami CXX)

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    add_compile_options(-permissive-)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("USAMI_DEBUG_MODE = 1")
    add_compile_definitions("USAMI_DEBUG_MODE=1")
endif()

set(QUICK_IMGUI_BACKEND "GLFW")
add_subdirectory(external/QuickImGui)

add_subdirectory(./usami-common)
add_subdirectory(./usami-raster)
add_subdirectory(./usami-ray)
add_subdirectory(./usami-shell)
add_subdirectory(./usami-shell-gui)