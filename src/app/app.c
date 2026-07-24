

#include "statemachine.h"
#include "fifo_test.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"
#include "data.h"
#include "gpio.h"
#include "uart.h"
#include "error.h"

volatile uint8_t g_start = 0;
volatile uint8_t g_error_ext = 0;
volatile uint8_t g_error_int = 0;
uint8_t g_temp = 0;
volatile uint8_t g_failure = 0;

static void app_int_fail(void);
static void app_int_fail(void);
/*
// Define a structure in a memory section that the linker won't clear on reset
__attribute__((section(".noinit"))) struct SystemStatus {
    uint32_t magic_number;
    uint32_t last_error_code;
    uint8_t reset_cnt;
} sys_status;

#define MAGIC_CRASH_FLAG 0xDEADBEEF
#define MAGIC_CLEAN_BOOT 0x12345678

void app_error_handler(void)
{
// 1. Disable all interrupts to prevent nesting
    __disable_irq();

    sys_status.magic_number = MAGIC_CRASH_FLAG;
    
    // 3. Force an immediate hardware reset (ARM Cortex-M example)
    NVIC_SystemReset(); 

}
*/
void app_init()
{
    /*
 // 1. Check if the magic flag matches our known crash flag
    if (sys_status.magic_number == MAGIC_CRASH_FLAG) {
        // We reached here via NVIC_SystemReset() from our error interrupt
        sys_status.reset_cnt++;

        if (sys_status.reset_cnt == 3) {
            g_failure = 1;
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
*/

    error_handler_init();

    // Initialize Peripherals
    gpio_led2_init();
    uart_init();
    i2c_init();
    rtc_set_time(30, 15, 3);
    tim2_init();
    adc_init();
    gpio_init_button();
    gpio_d2_init();
    tim6_init();
    tim7_init();
    tim17_init();
    
    // Check for internal failures
//    app_int_fail();
}

void app_standby()
{
    NVIC_DisableIRQ(TIM6_DAC_IRQn);
    NVIC_DisableIRQ(TIM7_IRQn);
    NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);

    while(!g_start) {
        // commenting this line out for debug purpose -> visability of state variables
//        __WFI(); // CPU should sleep here and wait for interuppts
    }

    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}

void app_run()
{
    fifo_test();
    random_test();
    gpio_led2_toggle();  

    tim2_delay_ms(500);
    printf("app_run()\n");
    rtc_get_time(rtc_time);

    g_temp = gpio_button_get();
}
void app_error(void)
{
    if (g_error_int) {
        while(!g_start) {}
        //app_error_handler();
        error_handler_reset();
    }
    
    if (g_error_ext) {
        while(!g_start) {}
    }
}


input_t app_input(void)
{
    // Start with STOP
    input_t input = INPUT_STOP;
    
    // Check for start condition
    if (g_start) {
        input = INPUT_START;
    }

    // Check for failures
//    error_check();
    if (g_error_ext || g_error_int) {
        input = INPUT_FAIL;
        g_start = 0;
    }

    return input;
}