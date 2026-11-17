include(FetchContent)

# doctest: single-header test framework
set(DOCTEST_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(doctest
    GIT_REPOSITORY  https://github.com/doctest/doctest.git
    GIT_TAG         v2.5.3
    GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(doctest)
# Provides doctest_discover_tests()
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# Single test executable: doctest collects every TEST_CASE linked into it
add_executable(pikaball_tests
    main.cpp
    test_physics.cpp
)
target_include_directories(pikaball_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pikaball_tests PRIVATE
    ${PROJECT_NAME}_physics
    doctest::doctest
)
target_compile_features(pikaball_tests PRIVATE cxx_std_20)
target_compile_options(pikaball_tests PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wpedantic>
)

# Register each TEST_CASE as its own CTest test
doctest_discover_tests(pikaball_tests)
