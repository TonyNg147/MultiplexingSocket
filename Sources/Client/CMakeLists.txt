project(Client)
set(SOURCES client.c)
add_executable(Client ${SOURCES})
set_target_properties(Client
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../Deliveries" 
)