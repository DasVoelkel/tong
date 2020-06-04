cmake_minimum_required(VERSION 3.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(the_framework LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ./${CMAKE_PROJECT_NAME})

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED IMPORTED_TARGET
    allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5
    allegro_acodec-5 allegro_image-5 allegro_color-5 allegro_ttf-5
    allegro_dialog-5
)

add_subdirectory(lib)

file(GLOB_RECURSE SRCS src/*.cpp)
add_executable(${PROJECT_NAME} ${SRCS} include/core/threads/GuiThread.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    agui_allegro5 PkgConfig::ALLEGRO
)




