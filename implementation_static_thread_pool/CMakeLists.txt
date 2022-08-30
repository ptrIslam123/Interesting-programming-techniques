cmake_minimum_required(VERSION 3.23)
project(implementation_static_thread_pool)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp static_thread_pool.cpp)

target_link_libraries(${PROJECT_NAME} pthread)
