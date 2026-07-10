#include "main.h"
#include "fifo_test.h"
#include "tx_api.h"
#include "tx_app.h"
#include "tests.h"
#include "rtc.h"
#include "adc.h"
#include "data.h"


int main(void) {

    // Initialize Peripherals
    mcu_init();
    UART1_Init();
    i2c_init();
    rtc_set_time(30, 15, 3);
    
    tim2_init();
    fifo_test();
    random_test();

    adc_init();
    
    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();



    while(1) {
        // Fetch raw 12-bit sample (0 - 4095)
        adc_raw_value = adc_read();
        
        // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
        adc_input_voltage = ((float)adc_raw_value * 3.3f) / 4095.0f;
        

        tim2_delay_ms(500);
        printf("while loop\n");
        rtc_get_time(rtc_time);
    }

    return 0;
}
