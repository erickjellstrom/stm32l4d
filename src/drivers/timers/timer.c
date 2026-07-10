#include "timer.h"


void tim2_init(void) {

    // 1. Enable the clock for TIM2 in the RCC register
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // 1. Set your values (they go into the preload buffer)
    TIM2->PSC = 3999; // 1kHz timer @4MHz
    TIM2->ARR = 0xFFFFFFFF; // maximum value -> not needed by delay function

    // 2. Safely apply the values instantly
    TIM2->EGR = TIM_EGR_UG;   // Force the update

    // 3. WIPE OUT THE SIDE EFFECTS
    TIM2->SR = 0;             // Clear the Status Register completely

    // 4. Reset the counter register
    TIM2->CNT = 0;
    
    // 5. Enable the timer counter
    TIM2->CR1 |= TIM_CR1_CEN;
}

void tim2_delay_ms(uint32_t ms) {
    // Reset the counter to 0 before starting the delay
    TIM2->CNT = 0;
    
    // Wait until the counter reaches the desired millisecond value
    while (TIM2->CNT < ms) {
        // Do nothing, just loop until time passes
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