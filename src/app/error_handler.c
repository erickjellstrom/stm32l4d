
#include "adc.h"
#include "gpio.h"
#include "data.h"
#include "error_statemachine.h"
#include "reset_handler.h"

//static uint8_t internal_failure = 0;

//extern volatile uint8_t g_error_ext;
//extern volatile uint8_t g_error_int;
//extern volatile uint8_t g_failure;

static uint8_t error_check(void);
static error_input_t error_input(void);


error_input_t app_error_inp = INPUT_NO_ERROR;

extern struct error_statemachine* error_sm_ptr;

void sensor_loop(void)
{
    // set global variable d2 with button status
    gpio_d2 = gpio_d2_get();


    // Fetch raw 12-bit sample (0 - 4095)
    adc_raw_value = adc_read();
    
    // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
    adc_input_voltage = ((float)adc_raw_value * 3.3f) / 4095.0f;

}
/*
static uint8_t error_check(void)
{
    if(app_ext_fail() || app_int_fail()) return 1;
    else return 0;
}
*/
void error_loop(void)
{
    // Update Error Statemachine
    app_error_inp = error_input();
    error_sm_process_event(&error_sm_ptr, app_error_inp);
    error_sm_execute(error_sm_ptr);
}

/*
static uint8_t app_int_fail(void)
{
    uint8_t d2 = gpio_d2;

    g_error_int = 0;
    if (d2 == 1) {
        g_error_int = 1;
    }
    if (g_error_int == 1) return 1;
    else return 0;
}
*/

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
/*
static uint8_t app_ext_fail(void)
{

    if (adc_input_voltage < 1.5) {
        g_error_ext = 1;
    }
    else g_error_ext = 0;
    
    if (g_error_ext == 1) return 1;
    else return 0;
}
*/

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