


cmake_minimum_required(VERSION 3.1)
    INCLUDE(FindPkgConfig)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG)

project(tong LANGUAGES CXX VERSION 0.01)
add_compile_options(-std=c++17)


PKG_CHECK_MODULES( PKG REQUIRED allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5)

set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_PROJECT_NAME})




include_directories(include)


file(
    GLOB_RECURSE
    SRCS
    src/*
)
add_executable( ${PROJECT_NAME} ${SRCS} )



target_link_libraries(${CMAKE_PROJECT_NAME}
        PUBLIC "${PKG_LIBRARIES}")





