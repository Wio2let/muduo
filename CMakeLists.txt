cmake_minimum_required(VERSION 3.16)

project(muduo_from_scratch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(muduo_core
    src/Channel.cc
    src/CurrentThread.cc
    src/EPollPoller.cc
    src/EventLoop.cc
    src/Poller.cc
    src/Timestamp.cc
)

target_include_directories(muduo_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(muduo_core
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
        muduo_core
)

add_executable(stage1_eventfd
    example/stage1_eventfd.cc
)

target_link_libraries(stage1_eventfd
    PRIVATE
        muduo_core
)
