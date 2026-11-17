cmake_minimum_required(VERSION 3.25)
project(pikaball_revamped VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PIKA_EMBED_ASSETS "Embed game assets into the binary for a portable executable" OFF)
set(PIKA_ASSETS_INSTALL_DIR "assets" CACHE STRING "Install destination for external assets (relative to CMAKE_INSTALL_PREFIX)")
set(PIKA_ASSETS_RUNTIME_DIR "" CACHE STRING "Optional runtime directory used to find external assets")
option(PIKA_BUILD_TESTS "Build the unit tests" ON)

add_subdirectory(vendor)
add_subdirectory(src)

if (PIKA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
