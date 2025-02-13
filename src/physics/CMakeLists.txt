
set(PHYSICS_LIB_NAME "${PROJECT_NAME}_physics")
add_library(${PHYSICS_LIB_NAME} INTERFACE

        ball.cpp
    player.cpp
    physics.cpp
)
target_include_directories(${PHYSICS_LIB_NAME} INTERFACE
    ${CMAKE_SOURCE_DIR}/include
)
# The physics lib should not need any SDL stuff
#target_link_libraries(${PROJECT_NAME} PRIVATE
#    vendor  # Link to vendored SDL (and other libs if any)
#)
