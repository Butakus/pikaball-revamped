cmake_minimum_required(VERSION 3.25)
project(pikaball_revamped VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# By default, install into the build directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/pikaball-release" CACHE PATH "Install path prefix" FORCE)
endif()
include(GNUInstallDirs)

option(PIKA_EMBED_ASSETS "Embed game assets into the binary for a portable executable" OFF)
option(PIKA_SYSTEM_INSTALL "Install in the Unix layout (bin/, share/), e.g. for the Flatpak. Default: the executable and assets/ side by side" OFF)
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
