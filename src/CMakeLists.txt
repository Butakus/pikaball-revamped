
# Build the physics library
set(PHYSICS_LIB_NAME "${PROJECT_NAME}_physics")
add_subdirectory(physics)

# Build the controller base library
set(CONTROLLER_BASE_LIB_NAME "${PROJECT_NAME}_controller")
set(KEYBOARD_CONTROLLER_LIB_NAME "${PROJECT_NAME}_kb_controller")
set(COMPUTER_CONTROLLER_LIB_NAME "${PROJECT_NAME}_computer_controller")
add_subdirectory(controller)

add_executable(${PROJECT_NAME} WIN32)

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${WIN32}>:-static>
)

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    sdl_system.cpp
    game.cpp
    $<$<PLATFORM_ID:Windows>:${CMAKE_SOURCE_DIR}/assets/pikaball-revamped.rc>
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
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Embed binary data into the executable
include(${CMAKE_SOURCE_DIR}/cmake/embed_resources.cmake)
embed_resources(${PROJECT_NAME}
    INPUTS
    ${CMAKE_SOURCE_DIR}/assets/hello.txt
    ${CMAKE_SOURCE_DIR}/assets/images/sprite_sheet_new.png
    ${CMAKE_SOURCE_DIR}/assets/sounds/ball_ground.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/ball_hit.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/chu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pi.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pika.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pikachu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pipikachu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/bgm.mp3
)

# Link assets to build directory
set(link_src "${CMAKE_SOURCE_DIR}/assets")
set(link_dst "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
if(WIN32)
    # Windows: use copy (symlinks often require admin privileges)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${link_src}" "${link_dst}"
            COMMENT "Copying assets directory (Windows): ${link_src} -> ${link_dst}"
    )
else()
    # Unix-like: use symbolic link
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${link_src}" "${link_dst}"
            COMMENT "Creating symlink to assets directory: ${link_src} -> ${link_dst}"
    )
endif()

# Install targets
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)
#install(TARGETS
#    ${CONTROLLER_BASE_LIB_NAME}
#    ${KEYBOARD_CONTROLLER_LIB_NAME}
#    ${COMPUTER_CONTROLLER_LIB_NAME}
#    DESTINATION lib/${PROJECT_NAME}
#)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets" DESTINATION ${CMAKE_INSTALL_PREFIX})
