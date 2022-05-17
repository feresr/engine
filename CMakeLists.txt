cmake_minimum_required(VERSION 3.14.2)

project(engine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-2.0.20 # grab latest stable release
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2)

file(GLOB src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/image/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/math/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/opengl/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl.cpp
        )

add_library(engine STATIC ${src})

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/tmxlite/tmxlite)

target_include_directories(engine

        # PUBLIC
        # All the directories following PUBLIC will be used for the current target (engine) and the other targets
        # that have dependencies on the current target (sandbox)
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_SOURCE_DIR}/public/components
        ${CMAKE_CURRENT_SOURCE_DIR}/public/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/public/math
        # todo: remove /vendor/ from Public include sources
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/

        # Makes it possible to include "use" classes from sandbox
        # Interpretation:
        # All the include directories following INTERFACE will NOT be used for the current target but will be
        # accessible for the other targets that have dependencies on the current target.
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        # These headers won't be available from sandbox
        # .cpp files will look for header files in these locations when building the library
        # note: .h files in the library still need the absolute path
        # All the include directories following PRIVATE will be used for the current target only
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/src/image
        ${CMAKE_CURRENT_SOURCE_DIR}/src/math
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/
        )

# SDL and glad should be PRIVATE
target_link_libraries(engine PUBLIC glm SDL2-static spdlog glad PRIVATE tmxlite stb)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/")
add_subdirectory(sandbox)
