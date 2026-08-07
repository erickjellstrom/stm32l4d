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
#include "app_statemachine.h"
#include "app_handler.h"
#include "error_statemachine.h"
#include "interrupts.h"

float temperature_celsius;
int16_t raw_x;
int16_t raw_y;
int16_t raw_z;
int16_t raw_t;

uint8_t accel_buffer[6] = {0};
uint8_t temp_buffer[2] = {0};

int main(void) {

    // Initialize application statemachine and execute init state
    sm_init(&app_sm, APP_STATE_INIT);
    
    // Initialize error statemachine
    error_sm_init(&error_sm_ptr, STATE_NO_ERROR);
    
    // Execute sensor loop and error statemachine one time from main()
    sensor_loop();
    error_loop();
    
    // Start timers IRQs
    irq_init();

    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

    while(1) {
        tim2_delay_ms(500);
        printf("main while loop\n");
    }

    return 0;
}
