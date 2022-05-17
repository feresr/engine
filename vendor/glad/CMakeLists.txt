project(glad)

file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(glad STATIC ${src})

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)