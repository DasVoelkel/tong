


cmake_minimum_required(VERSION 3.1)
set(CMAKE_BUILD_TYPE Release)

set(TARGET tong)

project($(TARGET) VERSION 0.01)

include_directories(include)

add_subdirectory(src)
add_subdirectory(lib)

