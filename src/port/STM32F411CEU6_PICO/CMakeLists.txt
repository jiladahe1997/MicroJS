cmake_minimum_required(VERSION 3.10)



add_subdirectory(STM32CubeMX_generate/cmake/stm32cubemx)
add_library(port INTERFACE)
target_link_libraries(port INTERFACE stm32cubemx)
target_sources(port INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/letter_shell_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/xmodem_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeMX_generate/Core/Src/led_task.c
)

#LINK file is defined at file "compile_options.cmake"
set_target_properties(port PROPERTIES LINK_DEPENDS ${LINK_FILE})
