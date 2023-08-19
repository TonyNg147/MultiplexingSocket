project(SocketUtil)
set(SOURCES Srcs/ServerManip.cpp)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Abstract)
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/../Deliveries"
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)