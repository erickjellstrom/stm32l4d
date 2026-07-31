
#include "adc.h"
#include "gpio.h"
#include "data.h"
#include "error_statemachine.h"
#include "reset_handler.h"


static error_input_t error_input(void);
error_input_t app_error_inp = INPUT_NO_ERROR;

void error_loop(void)
{
    // Update Error Statemachine
    app_error_inp = error_input();
    error_sm_process_event(&error_sm_ptr, app_error_inp);
}

static uint8_t error_check_internal(void)
{
    if (gpio_d2 == 1) {
        return 1;
    }
    else return 0;
}

static uint8_t error_check_external(void)
{
    if (adc_input_voltage < 1.5) {
        return 1;
    }
    else return 0;
}    

static error_input_t error_input(void)
{
    // Start with no error 
    error_input_t input = INPUT_NO_ERROR;
    
    // Check for external errors
    if (error_check_external()) {
        input = INPUT_EXT_ERROR;
    }

    // Check for internal errors
    if (error_check_internal()) {
        input = INPUT_INT_ERROR;
    }

    // Check for sw resets due to internal errors
    if (sys_status.reset_cnt >= 3) {
        input = INPUT_INT_FAILURE;
    }

    // TBD check for failures after power on (nvm implementation needed)
    //input = INPUT_INT_PERM_FAILURE;

    return input;
}