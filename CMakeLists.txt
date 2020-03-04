


cmake_minimum_required(VERSION 3.1)
set(CMAKE_BUILD_TYPE Release)

project(tong VERSION 0.01)
set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_PROJECT_NAME})

include_directories(include)


file(
    GLOB_RECURSE
    SRCS
    src/*
)
add_executable( ${PROJECT_NAME} ${SRCS} )

target_link_libraries(${CMAKE_PROJECT_NAME}
        PUBLIC -lpthread -lallegro)




set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)
