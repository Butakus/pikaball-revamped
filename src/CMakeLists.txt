
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
    $<$<PLATFORM_ID:Windows>:-static>
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
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23 c_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wpedantic>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-DPIKA_DEBUG>
)

# Embed resources into binary using custom version of battery::embed
include(${CMAKE_SOURCE_DIR}/cmake/pika_embed.cmake)
pika_embed(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/assets/images/sprite_sheet.png
    ${CMAKE_SOURCE_DIR}/assets/sounds/bgm.mp3
    ${CMAKE_SOURCE_DIR}/assets/sounds/pi.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pika.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/chu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pikachu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/pipikachu.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/ball_hit.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/ball_ground.wav
    ${CMAKE_SOURCE_DIR}/assets/sounds/trex.ogg
    ${CMAKE_SOURCE_DIR}/assets/font.ttf
)

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
