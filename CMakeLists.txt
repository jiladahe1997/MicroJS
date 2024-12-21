cmake_minimum_required(VERSION 3.10)

# check if user have choose board
OPTION(PORT "Choose which board(chip) to use, MicroJS is Binding to dedicated board or chip" OFF)
if(NOT PORT)
    message(FATAL_ERROR 
    "  MicroJS is Binding to board or chip, Please choose a board or chip\n"
    "  For example: Use \"cmake ../ -DPORT=STM32F411CEU6_PICO\""
    )
endif()

# check if board path exits
set(port_path "${CMAKE_CURRENT_SOURCE_DIR}/src/port/${PORT}")
if(NOT EXISTS ${port_path} )
    message(FATAL_ERROR 
    " PORT path: ${port_path} is not exist"
    )
endif()

# include port compile options
include(${port_path}/compile_options.cmake)

message(NOTICE "CMAKE_C_COMPILER is ${CMAKE_C_COMPILER}")
message(NOTICE "CMAKE_CXX_COMPILER is ${CMAKE_CXX_COMPILER}")

project(MicroJS)
enable_language(ASM)

# add port lib
add_subdirectory(${port_path})

add_executable(MicroJS)
target_link_libraries(MicroJS port)