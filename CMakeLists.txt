cmake_minimum_required(VERSION 4.3.1)

project (ITCHParser
         VERSION 0.1
         )

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main
    src/main.cpp
    src/OrderBook.cpp
    )

target_compile_options(main
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    )

target_link_libraries(main
    PRIVATE
    dl
    )
