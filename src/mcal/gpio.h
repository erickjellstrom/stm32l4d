
#include "stm32l475xx.h"

void gpio_led2_init(void);

static inline void gpio_led2_toggle(void) {
    // Toggle PB14 using the Output Data Register
    GPIOB->ODR ^= GPIO_ODR_OD14;
}

void gpio_init_button(void);
uint8_t gpio_button_get(void);