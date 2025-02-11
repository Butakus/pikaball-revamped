
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    window.cpp
    game.cpp
    view/wave.cpp
    view/cloud.cpp
    view/view.cpp
    view/volley_view.cpp
    view/intro_view.cpp
    view/menu_view.cpp
    physics/ball.cpp
    physics/player.cpp
    physics/physics.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
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
