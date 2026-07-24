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


// Main loop - only executes state when input has changed
void main_loop(void)
{
    input_t new_app_inp = app_input();
    // Update main Statemachine
    sm_process_event(&app_sm, new_app_inp); 
    sm_execute(app_sm);
}

int main(void) {

    // Initialize main statemachine
    sm_init(&app_sm, STATE_IDLE);
    sm_execute(app_sm);
    
    // Initialize error statemachine
    error_sm_init(&error_sm_ptr, STATE_NO_ERROR);
    error_sm_execute(error_sm_ptr);
    
    // Set priority for TIM6 DAC underflow interrupt (optional but recommended)
    NVIC_SetPriority(TIM6_DAC_IRQn, 5);

    // Enable TIM6 interrupt line in the NVIC
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    
    // Set priority for TIM7 DAC underflow interrupt (optional but recommended)
    NVIC_SetPriority(TIM7_IRQn, 4);

    // Enable TIM7 interrupt line in the NVIC
    NVIC_EnableIRQ(TIM7_IRQn);

    // 6. Configure the Nested Vectored Interrupt Controller (NVIC)
    // TIM17 shares an interrupt line on many STM32L4 MCUs: TIM1_TRG_COM_TIM17_IRQn
    NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 3); 
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

    while(1) {
        tim2_delay_ms(500);
        printf("main while loop\n");
    }

    return 0;
}
