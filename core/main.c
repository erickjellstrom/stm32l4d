#include "main.h"
#include "fifo_test.h"
#include "tx_api.h"
#include "tx_app.h"
#include "tests.h"
#include "i2c.h"
#include "rtc.h"


uint8_t time_buffer[3]; 

int main(void) {

    // Initialize Peripherals
    mcu_init();
    UART1_Init();
    i2c_init();
    set_rtc_time(30, 15, 10);
    
    tim2_init();
    fifo_test();
    random_test();

    //Hand over full CPU control to the ThreadX RTOS kernel
//    tx_kernel_enter();

    while(1) {
        tim2_delay_ms(500);
        printf("while loop\n");
        get_rtc_time(time_buffer);
    }

    return 0;
}
