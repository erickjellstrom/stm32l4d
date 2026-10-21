cmake_minimum_required(VERSION 3.20)

# 1. Project Configuration
project(stm32l4d LANGUAGES C ASM)

# 2. Options and Build Types
set(INTERFACE "HW" CACHE STRING "Interface mode: HW or GDB")
set_property(CACHE INTERFACE PROPERTY STRINGS "HW" "GDB")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
endif()

# 3. Target MCU Compile Flags
set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

set(COMMON_FLAGS
    ${MCU_FLAGS}
    -Wall
    -fdata-sections
    -ffunction-sections
)

# Apply Debug/Release Optimization equivalent to your Makefile
set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)

# 4. Define the Executable and Sources
add_executable(${PROJECT_NAME}.elf
    # C Sources
    src/core/main.c
    src/core/system_stm32l4xx.c
    src/core/sysmem.c
    src/core/syscalls.c
    src/core/interrupts.c
    src/app/app_handler.c
    src/app/error_handler.c
    src/app/reset_handler.c
    src/app/sensor_handler.c
    src/app/error_statemachine.c
    src/app/app_statemachine.c
    src/app/feat_imu_logger.c
    src/data/data.c
    src/drivers/mcu/mcu.c
    src/mcal/timer.c
    src/mcal/i2c.c
    src/mcal/i2c_sens.c
    src/mcal/adc.c
    src/mcal/uart.c
    src/mcal/gpio.c
    src/ecal/pcf8563.c
    src/ecal/sensor.c
    src/utils/fifo/fifo.c
    src/utils/fifo/fifo_test.c
    src/utils/random/random.c
    test/tests.c
    src/threadx/app/tx_app.c
    # Assembly Sources
    startup_stm32l475xx.s
    src/core/tx_initialize_low_level.S
)

# 5. Include Directories
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    src/core
    src/data
    src/app
    src/drivers/CMSIS/Device/ST/STM32L4xx/Include
    src/drivers/CMSIS/Core/Include
    src/drivers/mcu
    src/mcal
    src/common
    src/drivers
    src/ecal
    src/utils/fifo
    src/utils/random
    test
    src/threadx/common/inc
    src/threadx/app
    src/threadx/ports/cortex_m4/gnu/inc
)

# 6. Preprocessor Definitions
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32L475xx)

if(INTERFACE STREQUAL "GDB")
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE USE_GDB_INTERFACE)
else()
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE USE_HW_GPIO)
endif()

# Apply common flags
target_compile_options(${PROJECT_NAME}.elf PRIVATE ${COMMON_FLAGS})

# 7. Linker Configurations
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32L475XX_FLASH.ld)
set(THREADX_LIB ${CMAKE_CURRENT_SOURCE_DIR}/src/threadx/libthreadx.a)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -specs=nano.specs
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

# Link Precompiled ThreadX static library along with standard C and math math libraries
target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    ${THREADX_LIB}
    c
    m
    nosys
)

# 8. Post-Build Commands: Hex, Bin, and Size Report
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT "Generating .hex, .bin, and computing text/data sizes..."
)

# 9. Flash target using OpenOCD
add_custom_target(flash
    COMMAND openocd -f interface/stlink.cfg -f target/stm32l4x.cfg -c "program ${PROJECT_NAME}.bin verify reset exit 0x08000000"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing target device..."
)


