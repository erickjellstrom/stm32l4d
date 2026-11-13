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
#include "feat_imu_logger.h"


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
    gpio_swo_init();
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
    tim3_init();
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

    printf("press blue button\n");
    while(!g_start) {
        // commenting this line out for debug purpose -> visability of state variables
//        __WFI(); // CPU should sleep here and wait for interuppts
    }

    //log_start();

    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
}
/* Send 1-byte marker (0xaa = HIGH, 0xbb = LOW) to ITM Port 0 */
static void ITM_SendMarker(uint8_t marker)
{
    if (((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0UL) &&
        ((ITM->TER & 1UL) != 0UL))
    {
        while (ITM->PORT[0].u32 == 0UL)
        {
            __NOP();
        }
        ITM->PORT[0].u8 = marker;
    }
}
void app_run()
{
    
    //ITM_SendMarker(0xaa); /* Channel 0 goes HIGH */
    //for (volatile uint32_t i = 0; i < 100000; i++) { __NOP(); }

    //ITM_SendMarker(0xbb); /* Channel 0 goes LOW */
    //for (volatile uint32_t i = 0; i < 100000; i++) { __NOP(); }


//    fifo_test();
//    random_test();
//    gpio_led2_toggle();  

    tim2_delay_ms(50);
//    printf("app_run()\n");
//    rtc_get_time(rtc_time);

//    print_imu();
//    sensor_temp_read_bm();
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
