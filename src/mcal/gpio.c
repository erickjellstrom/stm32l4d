#include "gpio.h"
#include "data.h"


void gpio_swo_init(void)
{
    /* 1. Enable GPIOB clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    (void)RCC->AHB2ENR; /* Read-back delay */

    /* 2. Set PB3 to Alternate Function mode (MODER[7:6] = 0b10) */
    GPIOB->MODER &= ~GPIO_MODER_MODE3_Msk;
    GPIOB->MODER |= GPIO_MODER_MODE3_1;

    /* 3. Set PB3 Alternate Function to AF0 (SYS_AF / TRACESWO) */
    GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL3_Msk;

    /* 4. Set PB3 output speed to Very High for clean SWO edges */
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3_Msk;
}


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
    #ifdef USE_GDB_INTERFACE
    return g_board_inputs.digital_switch_1;  
    #else
    if (GPIOD->IDR & GPIO_IDR_ID14) return 1;
    else return 0;
    #endif
}

void gpio_button_init(void) {
    // 1. Enable GPIOC and SYSCFG Clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   // Enable Port C clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // Enable SYSCFG clock
    
    // 2. Configure PC13 as Input Mode
    GPIOC->MODER &= ~(GPIO_MODER_MODE13);
    
    // no pull up or pull down
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
}


uint8_t gpio_button_get(void)
{
//    uint8_t ret;

    if (GPIOC->IDR & GPIO_IDR_ID13) {
        return 1;
    }
    else return 0; 
}


