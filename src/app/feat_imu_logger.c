
#include "stm32l475xx.h"
#include "data.h"

void log_sensor()
{
    sensor_imu_read_bm();
    printf("%d,%d,%d,%d,%d\n",log_nbr, raw_x, raw_y, raw_z, letter_cmd);

    log_counter++;
}

void log_start_timer()
{
    printf("input a letter via uart\n");
    console_cmd = USART1_Read();
    printf("console_cmd: %d\n", console_cmd);
    
    log_counter = 0;
    // Timer 3 @10kHz
    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);

    while(log_counter < 300) {}
    
    NVIC_DisableIRQ(TIM3_IRQn);
}

void log_start()
{
    printf("input a letter via uart\n");
    letter_cmd = USART1_Read();
    printf("letter_cmd: %d\n", letter_cmd);

    printf("input number of runs n via uart\n");
    uint8_t num_cmd = USART1_Read();
    printf("num_cmd: %d\n", num_cmd); 

    for (uint8_t i=0; i<num_cmd; i++) {
        log_start_button();
    }

}

void log_start_button()
{
    log_counter = 0;
    log_nbr++;

    g_start = 0;
    printf("start run with blue button\n");
    while(!g_start) {}
    
    // Timer 3 @10kHz
    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);

    while(log_counter < 200) {}
    NVIC_DisableIRQ(TIM3_IRQn);
    
}