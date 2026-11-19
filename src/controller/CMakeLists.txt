add_library(${CONTROLLER_BASE_LIB_NAME} INTERFACE)

target_include_directories(${CONTROLLER_BASE_LIB_NAME} INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)

# Keyboard controller module
add_library(${KEYBOARD_CONTROLLER_LIB_NAME}
    keyboard_controller.cpp
)
target_include_directories(${KEYBOARD_CONTROLLER_LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${KEYBOARD_CONTROLLER_LIB_NAME}
    PUBLIC ${CONTROLLER_BASE_LIB_NAME}
    PRIVATE pika_warnings
)
target_compile_features(${KEYBOARD_CONTROLLER_LIB_NAME} PRIVATE cxx_std_23)

# Computer controller module (original computer mode)
add_library(${COMPUTER_CONTROLLER_LIB_NAME}
    computer_controller.cpp
)
target_include_directories(${COMPUTER_CONTROLLER_LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${COMPUTER_CONTROLLER_LIB_NAME}
    PUBLIC ${CONTROLLER_BASE_LIB_NAME}
    PRIVATE pika_warnings
)
target_compile_features(${COMPUTER_CONTROLLER_LIB_NAME} PRIVATE cxx_std_23)
