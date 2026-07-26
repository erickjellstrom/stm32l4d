#include "app_statemachine.h"
#include "error_statemachine.h"
#include "fifo_test.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"
#include "data.h"
#include "gpio.h"
#include "uart.h"
#include "error_handler.h"
#include "reset_handler.h"

volatile uint8_t g_start = 0;
volatile uint8_t g_error_ext = 0;
volatile uint8_t g_error_int = 0;
uint8_t g_temp = 0;
volatile uint8_t g_failure = 0;

static app_input_t app_calc_input(void);

struct app_statemachine* app_sm;
app_input_t app_inp = INPUT_STOP;

// Main loop - only executes state when input has changed
void app_loop(void)
{
    app_input_t new_app_inp = app_calc_input();
    // Update main Statemachine
    sm_process_event(&app_sm, new_app_inp); 
    sm_execute(app_sm);
}

static app_input_t app_calc_input(void)
{
    // Start with STOP
    app_input_t input = INPUT_STOP;
    
    // Check for start condition
    if (g_start) {
        input = INPUT_START;
    }

    // Check for failures
    if (error_sm_ptr->error_state != STATE_NO_ERROR) {
        input = INPUT_FAIL;
        g_start = 0;
    }

    return input;
}

void app_init()
{
    reset_handler_init();

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
//    fifo_test();
//    random_test();
    gpio_led2_toggle();  

    tim2_delay_ms(500);
    printf("app_run()\n");
//    rtc_get_time(rtc_time);

//    g_temp = gpio_button_get();
}

void app_error(void)
{
    printf("main statemachine error state function\n");
    
    // External error 
    while (error_sm_ptr->error_state == STATE_EXT_ERROR) {}
        // stay in loop

    // Internal error
    if (error_sm_ptr->error_state == STATE_INT_ERROR) {
        while(!g_start) {} // wait for button to be pressed before reset
        reset_handler();
    }

    // Internal failure
    while (error_sm_ptr->error_state == STATE_INT_FAILURE) {}
        // stay in loop
}
