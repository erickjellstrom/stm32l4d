#include "main.h"
#include "fifo_test.h"
#include "tx_api.h"
#include "tx_app.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"
#include "data.h"
#include "gpio.h"
#include "uart.h"
#include "statemachine.h"

// Use 'extern' to tell main.c that app_sm is defined in another file
extern const struct statemachine app_sm[];

int main(void) {

    // Initialize Peripherals
    gpio_led2_init();
    uart_init();
    i2c_init();
    rtc_set_time(30, 15, 3);
    
    tim2_init();
    fifo_test();
    random_test();

    adc_init();
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

// Start at State 0
    const struct statemachine* current = &app_sm[0];
    printf("Starting State: %d\n", current->state);

    // Simulate an input of '1' while in State 0 -> should go to State 1
    int input = 1; 
    current = current->next[input];
    printf("New State: %d\n", current->state); // Prints 1

    // Simulate an input of '2' while in State 1 -> should go to State 2
    input = 2;
    current = current->next[input];
    printf("New State: %d\n", current->state); // Prints 2


    while(1) {
        // Fetch raw 12-bit sample (0 - 4095)
        adc_raw_value = adc_read();
        
        // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
        adc_input_voltage = ((float)adc_raw_value * 3.3f) / 4095.0f;
        gpio_led2_toggle();  

        tim2_delay_ms(500);
        printf("while loop\n");
        rtc_get_time(rtc_time);
    }

    return 0;
}
