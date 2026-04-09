cmake_minimum_required(VERSION 4.3.1)

project (ITCH_Viewer
         VERSION 0.1
         )

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/OrderBook.cpp
    )

add_subdirectory(contribs/clickhouse-cpp)


target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    )

target_include_directories(${PROJECT_NAME}
    PRIVATE
    contribs/clickhouse-cpp
    contribs/clickhouse-cpp/contrib/absl
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    dl
    clickhouse-cpp-lib
    )
