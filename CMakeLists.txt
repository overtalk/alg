cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

project(demo)

FILE(GLOB SC_FILES "*.cpp" "*.hpp")

add_executable(${PROJECT_NAME} ${SC_FILES})
