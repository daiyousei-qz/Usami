
add_executable(usami-shell-gui ./src/main.cpp)

target_link_libraries(usami-shell-gui 
    PRIVATE usami-common usami-raster usami-ray)

target_link_libraries(usami-shell-gui
    PRIVATE quick-imgui)