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

project(MicroJS ASM C CXX)
add_executable(MicroJS)
set_target_properties(MicroJS PROPERTIES SUFFIX ".elf")
#手动添加linkscript依赖，否则修改了ld文件，cmake检测不到
set_target_properties(MicroJS PROPERTIES LINK_DEPENDS ${LINK_FILE})




# add FreeRTOS-kernel
add_library(freertos_config INTERFACE)
set(FREERTOS_PORT  GCC_ARM_CM4F)
set(FREERTOS_HEAP  5)
target_include_directories(freertos_config SYSTEM
    INTERFACE
    ${port_path}) # The config file directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/FreeRTOS-Kernel)





# add letter-shell
add_library(letter_shell INTERFACE)
target_sources(letter_shell INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/letter-shell/src/shell_cmd_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/letter-shell/src/shell_companion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/letter-shell/src/shell_ext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/letter-shell/src/shell.c
)
target_include_directories(letter_shell INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/letter-shell/src/)




# add jerrry-script
# note: jerry-script must be prebuilt by user
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/jerryscript/build/lib) 
    message(FATAL_ERROR
    "  jerrry-script MUST be prebuilt first\n"
    "  Please run:\n"
    "    ```\n"
    "    cd src/jerryscript\n"
    "    python tools/build.py --clean --toolchain=../arm32.cmake -v --linker-flag=\"--specs=nosys.specs\" --compile-flag=\"-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard\" --mem-heap=32 --jerry-port=OFF --jerry-cmdline=OFF\n"
    "    ```\n"
    )
endif()
target_link_directories(MicroJS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/jerryscript/build/lib)




# add port
add_subdirectory(${port_path})





# final
target_link_libraries(MicroJS port freertos_kernel letter_shell jerry-core jerry-ext)
add_custom_command(TARGET MicroJS POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary MicroJS.elf MicroJS.bin 
)
add_custom_command(TARGET MicroJS POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex MicroJS.elf MicroJS.hex
)
add_custom_command(TARGET MicroJS POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -D -S MicroJS.elf > MicroJS.objdump
)