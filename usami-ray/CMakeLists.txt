
add_library(usami-ray STATIC)

file(GLOB_RECURSE USAMI_RAY_HEADERS "include/usami/ray/*.h")
file(GLOB_RECURSE USAMI_RAY_INTERNAL_HEADERS "src/usami/ray/*.h")
file(GLOB_RECURSE USAMI_RAY_SOURCES "src/usami/ray/*.cpp")
target_sources(usami-ray
    PUBLIC ${USAMI_RAY_HEADERS}
    PRIVATE ${USAMI_RAY_INTERNAL_HEADERS} ${USAMI_RAY_SOURCES})

target_include_directories(usami-ray
    PUBLIC "include"
    PRIVATE "src")

target_link_libraries(usami-ray
    PUBLIC usami-common)

find_package(TBB CONFIG REQUIRED)
find_package(embree 3 CONFIG REQUIRED)
target_link_libraries(usami-ray PUBLIC embree)