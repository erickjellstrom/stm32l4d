
# 1. Configure the project using the cross-compilation toolchain
cmake -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-toolchain.cmake -DINTERFACE=HW

:# 2. Compile the binaries
cmake --build build

# 3. Flash your STM32 MCU
cmake --build build --target flash


# 1. Update the configuration cache with GDB
cmake -B build -DINTERFACE=GDB

# 2. Compile (CMake will automatically re-compile files affected by this macro change)
cmake --build build
