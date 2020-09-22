
add_library(usami-common STATIC)

file(GLOB_RECURSE USAMI_COMMON_HEADERS "include/usami/*.h")
file(GLOB_RECURSE USAMI_COMMON_INTERNAL_HEADERS "src/usami/*.h")
file(GLOB_RECURSE USAMI_COMMON_SOURCES "src/usami/*.cpp")
target_sources(usami-common
    PUBLIC ${USAMI_COMMON_HEADERS}
    PRIVATE ${USAMI_COMMON_INTERNAL_HEADERS} ${USAMI_COMMON_SOURCES})

target_include_directories(usami-common
    PUBLIC "include"
    PRIVATE "src")

find_package(fmt CONFIG REQUIRED)
target_link_libraries(usami-common PUBLIC fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(usami-common PUBLIC nlohmann_json)

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries(usami-common PRIVATE tinyobjloader::tinyobjloader)

find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(usami-common PRIVATE ${STB_INCLUDE_DIRS})