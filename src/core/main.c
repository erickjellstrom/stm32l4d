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
//extern const struct statemachine app_sm[];

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

    struct statemachine* machine;
    
    printf("--- Initializing State Machine ---\n");
    sm_init(&machine, STATE_IDLE);

    printf("\n--- Simulating Event Pipeline ---\n");
    
    sm_execute(machine);

    // 1. Send START event
    printf("Sending START event:\n");
    sm_process_event(&machine, INPUT_START); // Transitions to RUNNING
    sm_execute(machine);

    // 2. Send another START event (loops back to self)
    printf("\nSending START event again:\n");
    sm_process_event(&machine, INPUT_START); // Stays in RUNNING
    sm_execute(machine);

    // 3. Send FAIL event
    printf("\nSending FAIL event:\n");
    sm_process_event(&machine, INPUT_FAIL);  // Transitions to ERROR
    sm_execute(machine);

    // 4. Send STOP event (Recovery path)
    printf("\nSending STOP event:\n");
    sm_process_event(&machine, INPUT_STOP);  // Recovers back to IDLE
    sm_execute(machine);

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
