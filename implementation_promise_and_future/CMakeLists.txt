cmake_minimum_required(VERSION 3.23)
project(implementation_promise_and_future)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp generic_class_of_exception.cpp)

target_link_libraries(${PROJECT_NAME} pthread)
