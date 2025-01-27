
add_executable(${PROJECT_NAME}
        window.cpp
        window.hpp)
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    vendor  # Link to vendored SDL (and other libs if any)
)

# Link assets to build directory
set(link_src "${CMAKE_SOURCE_DIR}/assets")
set(link_dst "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND}
        -E create_symlink ${link_src} ${link_dst}
        DEPENDS ${link_dst}
        COMMENT "symbolic link ${link_src} => ${link_dst}"
)
