set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY") 
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY  arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP  arm-none-eabi-objdump)

set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g") #不然GDB没法调试
set(LINK_FILE "${CMAKE_SOURCE_DIR}/src/port/STM32F411CEU6_PICO/STM32CubeMX_generate/stm32f411ceux_flash.ld")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${LINK_FILE}\"")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
