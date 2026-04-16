cmake_minimum_required(VERSION 3.16)

project(muduo_from_scratch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(muduo_base
    src/CurrentThread.cc
    src/Timestamp.cc
)

target_include_directories(muduo_base
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(muduo_base
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

add_executable(stage0_bootstrap
    example/stage0_bootstrap.cc
)

target_link_libraries(stage0_bootstrap
    PRIVATE
        muduo_base
)
