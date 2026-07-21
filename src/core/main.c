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
#include "app.h"
#include "error_statemachine.h"



struct statemachine* app_sm;
input_t app_inp = INPUT_STOP;

struct error_statemachine* error_sm_ptr;
error_input_t app_error_inp = INPUT_NO_ERROR;

int main(void) {

    // Initialize main statemachine
    sm_init(&app_sm, STATE_IDLE);
    // Initialize error statemachine
    error_sm_init(&error_sm_ptr, STATE_NO_ERROR);
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

    while(1) {

        // Update main Statemachine
        sm_execute(app_sm);
        app_inp = app_input();
        sm_process_event(&app_sm, app_inp); 
        
        // Update Error Statemachine
        error_sm_execute(error_sm_ptr);
        app_error_inp = error_input();
        error_sm_process_event(&error_sm_ptr, app_error_inp);
        
        tim2_delay_ms(500);
        printf("main while loop\n");
    }

    return 0;
}
