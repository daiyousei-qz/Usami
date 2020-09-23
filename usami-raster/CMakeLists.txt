
add_library(usami-raster STATIC)

file(GLOB_RECURSE USAMI_RASTER_HEADERS "include/usami/*.h")
file(GLOB_RECURSE USAMI_RASTER_INTERNAL_HEADERS "src/usami/*.h")
file(GLOB_RECURSE USAMI_RASTER_SOURCES "src/usami/*.cpp")
target_sources(usami-raster
    PUBLIC ${USAMI_RASTER_HEADERS}
    PRIVATE ${USAMI_RASTER_INTERNAL_HEADERS} ${USAMI_RASTER_SOURCES})

target_include_directories(usami-raster
    PUBLIC "include"
    PRIVATE "src")

target_link_libraries(usami-raster
    PUBLIC usami-common)