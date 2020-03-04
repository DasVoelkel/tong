
configure_file(main.hpp main.hpp)

target_include_directories(tong PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )