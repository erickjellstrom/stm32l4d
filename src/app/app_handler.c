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
#include "sensor_handler.h"
#include "sensor.h"
#include "timer.h"

//volatile uint8_t g_start = 0;

static app_cmd_t app_calc_input(void);

app_cmd_t app_inp = APP_CMD_STANDBY;

void app_loop(void)
{
    app_cmd_t new_app_inp = app_calc_input();
    // Update main Statemachine
    sm_process_event(&app_sm, new_app_inp); 
}

static app_cmd_t app_calc_input(void)
{
    // Start with STOP
    app_cmd_t input = APP_CMD_STANDBY;
    
    // Check for start condition
    if (g_start) {
        input = APP_CMD_RUN;
    }

    // Check for failures
    if (error_sm_ptr->error_state != STATE_NO_ERROR) {
        input = APP_CMD_ERROR;
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
    i2c_sens_init();
    tim2_init();
    adc_init();
    sensor_button_init();
    gpio_d2_init();
    tim6_init();
    tim7_init();
    tim17_init();
    sensor_temp_init_bm();
    sensor_imu_init_bm();

    #ifdef USE_HW_GPIO
    rtc_set_time(30, 15, 3);
    #endif

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

//    console_cmd = USART1_Read();
//    printf("console_cmd: %d\n", console_cmd);

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

    sensor_temp_read_bm();
    sensor_imu_read_bm();

     // Stream them as simple, comma-separated integers
    printf("%d,%d,%d\n", raw_x, raw_y, raw_z);
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
