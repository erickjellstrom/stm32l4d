
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Define the cross-compiler prefixes
set(PREFIX arm-none-eabi-)
set(CMAKE_C_COMPILER ${PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${PREFIX}g++)

# Avoid executing test compilation checks that fail without linker scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_OBJCOPY ${PREFIX}objcopy CACHE INTERNAL "")
set(CMAKE_SIZE ${PREFIX}size CACHE INTERNAL "")