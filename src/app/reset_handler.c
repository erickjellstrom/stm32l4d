
#include "stm32l475xx.h"
#include <stdint.h>



#define MAGIC_CRASH_FLAG 0xDEADBEEF
#define MAGIC_CLEAN_BOOT 0x12345678

// Define a structure in a memory section that the linker won't clear on reset
__attribute__((section(".noinit"))) struct SystemStatus {
    uint32_t magic_number;
    uint32_t last_error_code;
    uint8_t reset_cnt;
} sys_status;

void reset_handler(void)
{
// 1. Disable all interrupts to prevent nesting
    __disable_irq();

    sys_status.magic_number = MAGIC_CRASH_FLAG;
    
    // 3. Force an immediate hardware reset (ARM Cortex-M example)
    NVIC_SystemReset(); 
}

void reset_handler_init(void)
{
 // 1. Check if the magic flag matches our known crash flag
    if (sys_status.magic_number == MAGIC_CRASH_FLAG) {
        // We reached here via NVIC_SystemReset() from our error interrupt
        sys_status.reset_cnt++;

        if (sys_status.reset_cnt == 3) {
            //g_failure = 1;
            while(1) {} // endless loop
        } 
    } 
    else {
        // Cold boot or power glitch (RAM contained random garbage or 0)
        // Explicitly initialize the variables for clean execution
        sys_status.magic_number = MAGIC_CLEAN_BOOT;
        sys_status.last_error_code = 0;
        sys_status.reset_cnt = 0;
    }
}