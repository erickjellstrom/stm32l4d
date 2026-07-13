#include "gpio.h"

void gpio_led2_init(void)
{
    // Enable the clock for GPIOB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // Set Pin 14 as an Output Pin
    GPIOB->MODER &= ~(GPIO_MODER_MODE14);
    GPIOB->MODER |= (1 << GPIO_MODER_MODE14_Pos);
}
