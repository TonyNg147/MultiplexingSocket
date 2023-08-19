project(Server)
set(SOURCES server_epoll.cpp)
add_executable(SERVER ${SOURCES})
set_target_properties(SERVER
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../Deliveries" 
)
target_link_directories(SERVER PRIVATE "${PROJECT_BINARY_DIR}/../Deliveries")
target_link_libraries(SERVER PRIVATE SocketUtil)
target_include_directories(SERVER PRIVATE ${PROJECT_SOURCE_DIR}/../Utils/Srcs)