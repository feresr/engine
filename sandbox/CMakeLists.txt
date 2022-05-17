project(sandbox)
file(GLOB src
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

add_executable(sandbox ${src})

target_link_libraries(sandbox PUBLIC engine)

#file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#add_custom_command(TARGET sandbox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_custom_command(TARGET sandbox PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
target_include_directories(sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)