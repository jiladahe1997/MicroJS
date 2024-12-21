cmake_minimum_required(VERSION 3.10)



add_subdirectory(STM32CubeMX_generate/cmake/stm32cubemx)
add_library(port INTERFACE)
target_link_libraries(port INTERFACE stm32cubemx)
