
configure_file(main.hpp main.hpp)

target_include_directories($(CMAKE_PROJECT_NAME) PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )