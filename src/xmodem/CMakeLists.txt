cmake_minimum_required(VERSION 3.10)

add_library(xmodem INTERFACE)

target_sources(xmodem INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/xmodem.c
)


message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(xmodem INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)