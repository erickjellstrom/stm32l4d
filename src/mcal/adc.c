
#include "adc.h"  

void adc_init(void) {
    // =================================================================
    // STEP 1: Enable Bus Clocks
    // =================================================================
    // Enable GPIOC clock (PC5 belongs to Port C)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    
    // Enable ADC peripheral clock 
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    // =================================================================
    // STEP 2: Configure PC5 to Analog Mode
    // =================================================================
    // Set PC5 mode bits to 11 (Analog Mode). Bit positions: 10 and 11.
    GPIOC->MODER |= (3UL << GPIO_MODER_MODE5_Pos);
    
    // Completely disable internal Pull-Up and Pull-Down resistors on PC5
    GPIOC->PUPDR &= ~(3UL << GPIO_PUPDR_PUPD5_Pos);

    // Close the analog switch for PC5 to connect the pin physically to the ADC
    GPIOC->ASCR |= (1UL << 5); 
    // =================================================================
    // STEP 3: Setup ADC Clock Distribution
    // =================================================================
    // Configure synchronous clock mode (HCLK/1). 
    // Since MSI = 4MHz, HCLK = 4MHz. ADC Clock will be exactly 4 MHz.

    ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
    ADC123_COMMON->CCR |= (1UL << ADC_CCR_CKMODE_Pos); // 01: HCLK / 1
    // =================================================================
    // STEP 4: Power Regulation & Warm-up
    // =================================================================
    // Clear DEEPPWD to take the ADC peripheral out of Deep-Power-Down mode
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    
    // Enable the internal ADC voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN;
    
    // Wait for the regulator to stabilize. At 4MHz, ~80 assembly cycles 
    // are more than enough to meet the mandatory 20-microsecond requirement.
    for(volatile uint32_t i = 0; i < 100; i++);

    // =================================================================
    // STEP 5: Hardware Calibration
    // =================================================================
    // Ensure the ADC is inactive (ADEN = 0) before executing calibration
    ADC1->CR &= ~ADC_CR_ADEN;
    
    // Select Single-Ended Input Mode calibration (ADCALDIF = 0)
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    
    // Trigger calibration
    ADC1->CR |= ADC_CR_ADCAL;
    
    // Block execution until the hardware auto-clears the ADCAL bit
    while (ADC1->CR & ADC_CR_ADCAL);

    // =================================================================
    // STEP 6: Sequence Sequence and Sampling Cycles
    // =================================================================
    // Configure Sequence Length = 1 conversion total (0000 inside L bits)
    ADC1->SQR1 &= ~ADC_SQR1_L;
    
    // Assign Rank 1 to Channel 14 (Mapped directly to PC5)
    ADC1->SQR1 &= ~ADC_SQR1_SQ1;
    ADC1->SQR1 |= (14UL << ADC_SQR1_SQ1_Pos);
    
    // Set Sample Time for Channel 14. 
    // Since 4 MHz gives a long clock period (250ns), a 12.5 cycle 
    // sample time (010 binary) provides 3.125 microseconds of holding time.
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP14;
    ADC1->SMPR2 |= (2UL << ADC_SMPR2_SMP14_Pos); // 010: 12.5 ADC clock cycles

    // =================================================================
    // STEP 7: Activate the ADC Peripheral
    // =================================================================
    // Clear the ADC Ready flag (ADRDY) by writing a 1 to it
    ADC1->ISR |= ADC_ISR_ADRDY;
    
    // Turn on the ADC core
    ADC1->CR |= ADC_CR_ADEN;
    
    // Wait until hardware confirms readiness by flipping the ADRDY flag
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint32_t adc_read(void) {
    // Force a conversion start via software trigger
    ADC1->CR |= ADC_CR_ADSTART;
    
    // Wait for the End of Conversion (EOC) flag to rise
    while (!(ADC1->ISR & ADC_ISR_EOC));
    
    // Read the Data Register. This memory read automatically wipes the EOC flag.
    return ADC1->DR;
}
