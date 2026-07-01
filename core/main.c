#include "main.h"
#include "fifo_test.h"
#include "tx_api.h"
#include "tx_app.h"
#include "tests.h"
#include "i2c.h"
#include "rtc.h"

// The standard library calls __io_putchar when you use printf.
// We override it to push characters out of the USART1 data register.
int __io_putchar(int ch) {
    // Wait until the Transmit Data Register is empty (TXE bit becomes 1)
    while (!(USART1->ISR & USART_ISR_TXE));
    
    // Write the character to the data register
    USART1->TDR = (ch & 0xFF);
    return ch;
}

void delay(volatile int count) {
    while (count--) {
        // Waste CPU cycles to create a delay
    }
}
/*
void init_timer1(void)
{
	volatile uint32_t dmy __attribute__ ((unused));
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	dmy = RCC->APB2ENR;

	TIM1->PSC = (41999 & TIM_PSC_PSC); // 2000 Hz
	TIM1->ARR = (1999 & 0xffff); // Timer1 = 1s
	TIM1->DIER |= TIM_DIER_UIE;

	__NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	TIM1->CR1 |= TIM_CR1_CEN;
}

void timer1_delay(uint32_t ms)
{
	// Delay in number of timer ticks
	uint32_t delay = (ms * 2) - 1;
	uint32_t cnt = TIM1->CNT;
	uint32_t cnt_limit = (cnt + delay) % TIM1->ARR;
	printf("cnt: %d%tcnt_limit: %d\n", cnt, cnt_limit);
	while (TIM1->CNT != cnt_limit) {};
}
*/





uint8_t tx_buffer[3]; 

uint8_t rx_buffer[3]; 
uint8_t bcd_buffer[3]; 
uint8_t time_buffer[3]; 

int main(void) {

    // Initialize Peripherals
    mcu_init();
    UART1_Init();

//    BareMetal_I2C1_Init();
    i2c_init();
    
    tx_buffer[0] = 0x04;
    tx_buffer[1] = 0x01;
    tx_buffer[2] = 0x0f;
    i2c_read_mem(PCF8563_7BIT_ADDR, 0x02, 3, rx_buffer);
    set_rtc_time(30, 15, 10);
    i2c_read_mem(PCF8563_7BIT_ADDR, 0x02, 3, rx_buffer);
    tim2_init();
    fifo_test();
    random_test();

    //Hand over full CPU control to the ThreadX RTOS kernel
 //   tx_kernel_enter();

    while(1) {
        tim2_delay_ms(500);
        printf("while loop\n");
        get_rtc_time(time_buffer);
        i2c_read_mem(PCF8563_7BIT_ADDR, REG_VL_SECONDS, 3, rx_buffer);
    }

    return 0;
}
