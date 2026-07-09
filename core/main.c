#include "main.h"
#include "fifo_test.h"
#include "tx_api.h"
#include "tx_app.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"

uint8_t time_buffer[3]; 
uint32_t raw_value = 0;
float input_voltage = 0.0f;

int main(void) {

    // Initialize Peripherals
    mcu_init();
    UART1_Init();
    i2c_init();
    set_rtc_time(30, 15, 1);
    
    tim2_init();
    fifo_test();
    random_test();

    adc_init();
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();



    while(1) {
        // Fetch raw 12-bit sample (0 - 4095)
        raw_value = adc_read();
        
        // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
        input_voltage = ((float)raw_value * 3.3f) / 4095.0f;
        

        tim2_delay_ms(500);
        printf("while loop\n");
        get_rtc_time(time_buffer);
    }

    return 0;
}
