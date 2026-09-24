
#include "stm32l475xx.h"
#include "app_handler.h"
#include "data.h"

void irq_init(void)
{
    // 6. Configure the Nested Vectored Interrupt Controller (NVIC)
    // TIM17 shares an interrupt line on many STM32L4 MCUs: TIM1_TRG_COM_TIM17_IRQn
    NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 3); 
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
    
    // Set priority for TIM7 DAC underflow interrupt (optional but recommended)
    NVIC_SetPriority(TIM7_IRQn, 4);

    // Enable TIM7 interrupt line in the NVIC
    NVIC_EnableIRQ(TIM7_IRQn);

    
    // Set priority for TIM6 DAC underflow interrupt (optional but recommended)
    NVIC_SetPriority(TIM6_DAC_IRQn, 5);

    // Enable TIM6 interrupt line in the NVIC
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void irq_button_init(void)
{
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