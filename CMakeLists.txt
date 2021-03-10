cmake_minimum_required(VERSION 3.16)

project(Usami CXX)

# c++ standard version
add_compile_options(/std:c++latest)
if (MSVC)
    add_compile_options(/std:c++latest)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()

add_compile_definitions("USAMI_DEBUG_MODE=1")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("USAMI_DEBUG_MODE = 1")
    set(USAMI_DEBUG_MODE ON)
    add_compile_definitions("USAMI_DEBUG_MODE=1")
endif()

if (MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
    
    add_compile_options(/permissive- /fp:fast)
endif()

set(QUICK_IMGUI_BACKEND "GLFW")
add_subdirectory(external/QuickImGui)

add_subdirectory(./usami-common)
add_subdirectory(./usami-raster)
add_subdirectory(./usami-ray)
add_subdirectory(./usami-shell)
add_subdirectory(./usami-shell-gui)