# Network play: protocol, UDP transport and connection handshake
add_library(${NET_LIB_NAME}
    protocol.cpp
    udp_transport.cpp
    connection.cpp
)
target_include_directories(${NET_LIB_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
# Asio is only used inside udp_transport.cpp, so consumers don't get its headers
target_link_libraries(${NET_LIB_NAME} PRIVATE asio pika_warnings)
target_compile_features(${NET_LIB_NAME} PRIVATE cxx_std_23)
