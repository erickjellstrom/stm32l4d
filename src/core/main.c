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



struct statemachine* app_sm;
input_t app_inp = INPUT_STOP;

int main(void) {

    sm_init(&app_sm, STATE_IDLE);
    
//    main_init(); 
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

    while(1) {
        sm_execute(app_sm);
        app_failures();
        app_inp = app_input();
        sm_process_event(&app_sm, app_inp); 
        
        tim2_delay_ms(500);
        printf("main while loop\n");
    }

/*
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

*/
    return 0;
}
