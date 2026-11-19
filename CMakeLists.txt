cmake_minimum_required(VERSION 3.25)
project(pikaball_revamped VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PIKA_EMBED_ASSETS "Embed game assets into the binary for a portable executable" OFF)
set(PIKA_ASSETS_INSTALL_DIR "assets" CACHE STRING "Install destination for external assets (relative to CMAKE_INSTALL_PREFIX)")
set(PIKA_ASSETS_RUNTIME_DIR "" CACHE STRING "Optional runtime directory used to find external assets")
option(PIKA_BUILD_TESTS "Build the unit tests" ON)
option(PIKA_WERROR "Treat compiler warnings as errors in the project's own targets (not in dependencies)" OFF)

if (MSVC)
    # Embedded assets are huge string literals (over MSVC's limits), and Windows builds link with -static.
    message(WARNING "MSVC is not supported. Use MinGW-w64 (GCC) on Windows. See docs/BUILDING.md")
endif()

# Warning flags for the project's own targets. Linked PRIVATE to each target, so vendored dependencies aren't affected.
add_library(pika_warnings INTERFACE)
target_compile_options(pika_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wno-missing-field-initializers
    $<$<BOOL:${PIKA_WERROR}>:-Werror>>
)

add_subdirectory(vendor)
add_subdirectory(src)

if (PIKA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
