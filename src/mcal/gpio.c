#include "gpio.h"
#include "app.h"

void gpio_led2_init(void)
{
    // Enable the clock for GPIOB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // Set Pin 14 as an Output Pin
    GPIOB->MODER &= ~(GPIO_MODER_MODE14);
    GPIOB->MODER |= (1 << GPIO_MODER_MODE14_Pos);
}

void gpio_d2_init(void)
{
    // D2 / PD14 as input pin
    
    // Enable the clock for GPIOD
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;

    // Set Pin 14 as an Input Pin
    GPIOD->MODER &= ~(GPIO_MODER_MODE14);

    // pull down
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD14;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD14_1; // 01: pull down
}

uint8_t gpio_d2_get(void)
{
    if (GPIOD->IDR & GPIO_IDR_ID14) return 1;
    else return 0;
}

void gpio_init_button(void) {
    // 1. Enable GPIOC and SYSCFG Clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   // Enable Port C clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // Enable SYSCFG clock
    
    // 2. Configure PC13 as Input Mode
    GPIOC->MODER &= ~(GPIO_MODER_MODE13);
    
    // no pull up or pull down
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;

    // 3. Map PC13 to EXTI Line 13
    // EXTICR[3] handles lines 12 to 15. Clear and set bits for EXTI13 to 0x02 (Port C)
    SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13);
    SYSCFG->EXTICR[3] |=  (SYSCFG_EXTICR4_EXTI13_PC);
    
    // 4. Configure EXTI Line 13 Trigger and Mask
    EXTI->IMR1  |= EXTI_IMR1_IM13;    // Unmask interrupt for EXTI13
    EXTI->RTSR1 |= EXTI_FTSR1_FT13;  // Trigger on Falling edge (Press)
    
    EXTI->FTSR1 &= ~(EXTI_RTSR1_RT13); // Disable Rising edge trigger
    
    // 5. Enable EXTI Line 15_10 Interrupt in NVIC
    NVIC_SetPriority(EXTI15_10_IRQn, 1); // Set a safe priority
    NVIC_EnableIRQ(EXTI15_10_IRQn);      // Enable the global interrupt
}

// 6. Interrupt Service Routine (ISR) for EXTI lines 10 to 15
void EXTI15_10_IRQHandler(void) {
    // Check if the interrupt came from Line 13
    if (EXTI->PR1 & EXTI_PR1_PIF13) {
        
        g_start ^= 1;
        
        // Clear the pending flag by writing a '1' to it
        EXTI->PR1 |= EXTI_PR1_PIF13;
    }
}

uint8_t gpio_button_get(void)
{
    uint8_t ret = GPIOC->IDR;
    ret = ret & GPIO_IDR_ID13;

    if (GPIOC->IDR & GPIO_IDR_ID13) {
        ret = 1;
    }
    else ret = 0; 

    return ret;
}


