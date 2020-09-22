
add_executable(usami-shell ./src/main.cpp)

target_link_libraries(usami-shell PRIVATE usami-common)