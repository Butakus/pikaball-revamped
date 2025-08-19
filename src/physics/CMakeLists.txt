add_library(${PHYSICS_LIB_NAME}
    ball.cpp
    player.cpp
    physics.cpp
)
target_include_directories(${PHYSICS_LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_features(${PHYSICS_LIB_NAME} PRIVATE cxx_std_20)