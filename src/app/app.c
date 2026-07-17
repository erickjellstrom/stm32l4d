

#include "statemachine.h"
#include "fifo_test.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"
#include "data.h"
#include "gpio.h"
#include "uart.h"

volatile uint8_t g_start = 0;
uint8_t g_error = 0;
uint8_t g_temp = 0;

void app_init()
{
    // Initialize Peripherals
    gpio_led2_init();
    uart_init();
    i2c_init();
    rtc_set_time(30, 15, 3);
    tim2_init();
    adc_init();
    gpio_init_button();
}

void app_standby()
{
    while(!g_start) {
        __WFI(); // CPU should sleep here and wait for interuppts
    }
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

void app_failures()
{
    // Fetch raw 12-bit sample (0 - 4095)
    adc_raw_value = adc_read();
    
    // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
    adc_input_voltage = ((float)adc_raw_value * 3.3f) / 4095.0f;

    if (adc_input_voltage < 1.5) {
        g_error = 1;
    }
    else g_error = 0;
}

input_t app_input()
{
    // Start with STOP
    input_t input = INPUT_STOP;
    
    // Check for start condition
    if (g_start) {
        input = INPUT_START;
    }

    // Check for failures
    if (g_error) {
        input = INPUT_FAIL;
    }

    return input;
}