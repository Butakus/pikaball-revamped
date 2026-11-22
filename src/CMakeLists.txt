
# Build the physics library
set(PHYSICS_LIB_NAME "${PROJECT_NAME}_physics")
add_subdirectory(physics)

# Build the controller base library
set(CONTROLLER_BASE_LIB_NAME "${PROJECT_NAME}_controller")
set(KEYBOARD_CONTROLLER_LIB_NAME "${PROJECT_NAME}_kb_controller")
set(COMPUTER_CONTROLLER_LIB_NAME "${PROJECT_NAME}_computer_controller")
add_subdirectory(controller)

add_executable(${PROJECT_NAME} WIN32)

# Static runtime on Windows (MinGW), so the .exe runs without extra DLLs. MSVC is not supported.
if (WIN32 AND NOT MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

# Windows resources (icon + version info). The version comes from project(VERSION).
configure_file(${PROJECT_SOURCE_DIR}/cmake/pikaball-revamped.rc.in ${PROJECT_BINARY_DIR}/pikaball-revamped.rc @ONLY)

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    sdl_system.cpp
    game.cpp
    $<$<PLATFORM_ID:Windows>:${PROJECT_BINARY_DIR}/pikaball-revamped.rc>
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    vendor  # Link to vendored SDL (and other libs if any)
    ${PHYSICS_LIB_NAME}
    ${CONTROLLER_BASE_LIB_NAME}
    ${KEYBOARD_CONTROLLER_LIB_NAME}
    ${COMPUTER_CONTROLLER_LIB_NAME}
)
target_link_libraries(${PROJECT_NAME} PRIVATE pika_warnings)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-DPIKA_DEBUG>
)

if (PIKA_EMBED_ASSETS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PIKA_EMBED_ASSETS)

    # Embed resources into binary using custom version of battery::embed
    include(${PROJECT_SOURCE_DIR}/cmake/pika_embed.cmake)
    pika_embed(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/assets/images/sprite_sheet.png
        ${PROJECT_SOURCE_DIR}/assets/sounds/bgm.mp3
        ${PROJECT_SOURCE_DIR}/assets/sounds/pi.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/pika.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/chu.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/pikachu.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/pipikachu.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/ball_hit.wav
        ${PROJECT_SOURCE_DIR}/assets/sounds/ball_ground.wav
        ${PROJECT_SOURCE_DIR}/assets/font.ttf
    )
endif()

if (NOT PIKA_EMBED_ASSETS)
    # Non-embedded builds load assets/ from the executable's directory.
    set(_ASSETS_SRC "${PROJECT_SOURCE_DIR}/assets")
    set(_ASSETS_DST "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
    if (WIN32)
        # Windows: copy (creating symlinks often needs admin rights). Refreshed every time the game is relinked.
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${_ASSETS_SRC}" "${_ASSETS_DST}"
            COMMENT "Copying assets to the build directory"
            VERBATIM
        )
    else()
        # Unix: symlink, so asset edits are picked up without rebuilding
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${_ASSETS_SRC}" "${_ASSETS_DST}"
            COMMENT "Linking assets into the build directory"
            VERBATIM
        )
    endif()
endif()

if (NOT PIKA_ASSETS_RUNTIME_DIR STREQUAL "")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PIKA_ASSETS_RUNTIME_DIR="${PIKA_ASSETS_RUNTIME_DIR}")
endif()

# Install targets
install(TARGETS ${PROJECT_NAME}
    DESTINATION .
)

if (NOT PIKA_EMBED_ASSETS)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/
            DESTINATION ${PIKA_ASSETS_INSTALL_DIR}
    )
endif()
