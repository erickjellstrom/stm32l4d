
#include "stm32l475xx.h"

// ----------------------------------------------------------------------------
// 2. Configure USART1 (115200 Baud, 8 Data bits, 1 Stop bit)
// ----------------------------------------------------------------------------
void uart_init(void) 
{
    // A. Enable Clocks for GPIOB and USART1
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // B. Configure PB6 and PB7 to use Alternate Function mode (Mode '10')
    GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOB->MODER |= (2 << GPIO_MODER_MODE6_Pos) | (2 << GPIO_MODER_MODE7_Pos);

    // C. Map PB6 and PB7 to Alternate Function 7 (AF7 is USART1)
    // AFR[0] handles pins 0-7. Each pin gets 4 bits.
    GPIOB->AFR[0] &= ~((0xFU << GPIO_AFRL_AFSEL6_Pos) | (0xFU << GPIO_AFRL_AFSEL7_Pos));
    GPIOB->AFR[0] |= (7 << GPIO_AFRL_AFSEL6_Pos) | (7 << GPIO_AFRL_AFSEL7_Pos);

    // D. Calculate and Set Baud Rate (115200)
    // Default clock is 4 MHz. 4,000,000 / 115,200 = ~34.72. 
    // In bare metal, we round 34.72 to 35 (hex 0x23).
    USART1->BRR = 35;

    // E. Enable USART1, Transmitter (TE), and Receiver (RE)
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}