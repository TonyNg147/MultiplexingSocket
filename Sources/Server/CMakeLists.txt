project(Server)
set(SOURCES server_epoll.cpp)
add_executable(SERVER ${SOURCES})
set_target_properties(SERVER
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../Deliveries" 
)