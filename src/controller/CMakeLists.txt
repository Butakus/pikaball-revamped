add_library(${CONTROLLER_BASE_LIB_NAME} INTERFACE)

target_include_directories(${CONTROLLER_BASE_LIB_NAME} INTERFACE
    ${CMAKE_SOURCE_DIR}/include
)

add_library(${KEYBOARD_CONTROLLER_LIB_NAME}
    keyboard_controller.cpp
)
target_include_directories(${KEYBOARD_CONTROLLER_LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${KEYBOARD_CONTROLLER_LIB_NAME} PUBLIC
    ${CONTROLLER_BASE_LIB_NAME}
)