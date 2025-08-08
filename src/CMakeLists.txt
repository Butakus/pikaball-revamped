
# Build the physics library
set(PHYSICS_LIB_NAME "${PROJECT_NAME}_physics")
add_subdirectory(physics)

# Build the controller base library
set(CONTROLLER_BASE_LIB_NAME "${PROJECT_NAME}_controller")
set(KEYBOARD_CONTROLLER_LIB_NAME "${PROJECT_NAME}_kb_controller")
set(COMPUTER_CONTROLLER_LIB_NAME "${PROJECT_NAME}_computer_controller")
add_subdirectory(controller)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    sdl_system.cpp
    game.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    vendor  # Link to vendored SDL (and other libs if any)
    ${PHYSICS_LIB_NAME}
    ${CONTROLLER_BASE_LIB_NAME}
    ${KEYBOARD_CONTROLLER_LIB_NAME}
    ${COMPUTER_CONTROLLER_LIB_NAME}
)

# Link assets to build directory
set(link_src "${CMAKE_SOURCE_DIR}/assets")
set(link_dst "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND}
        -E create_symlink ${link_src} ${link_dst}
        COMMENT "symbolic link ${link_src} => ${link_dst}"
)
