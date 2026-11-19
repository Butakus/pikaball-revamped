add_library(${PHYSICS_LIB_NAME}
    ball.cpp
    player.cpp
    physics.cpp
)
target_include_directories(${PHYSICS_LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(${PHYSICS_LIB_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PHYSICS_LIB_NAME} PRIVATE pika_warnings)
