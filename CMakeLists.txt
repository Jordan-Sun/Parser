cmake_minimum_required(VERSION 3.0.0)
project(Parser VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(Parser src/main.cpp src/component.cpp src/connection.cpp src/graph.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
