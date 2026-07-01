
#include "stm32l4xx.h"
#include <stdint.h>

#define PCF8563_7BIT_ADDR  0x51

void i2c_init(void) {
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;   // Supply power clock to GPIOB
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;  // Supply power clock to I2C1

    RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;  // Assert reset line
    for(volatile int i = 0; i < 200; i++);     // Wait for stabilization
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST; // De-assert reset line

    /*------------------------------------------------------------------
     * 3. CONFIGURE INTERFACE PINS (PB8 = SCL, PB9 = SDA)
     *------------------------------------------------------------------*/
    GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->MODER |=  ((2U << (8 * 2)) | (2U << (9 * 2)));

    GPIOB->OTYPER |=  ((1U << 8) | (1U << 9)); // Set open-drain flags

    GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

    GPIOB->AFR[1] &= ~((0xFU << ((8 - 8) * 4)) | (0xFU << ((9 - 8) * 4)));
    GPIOB->AFR[1] |=  ((4U << ((8 - 8) * 4))   | (4U << ((9 - 8) * 4)));

    /*------------------------------------------------------------------
     * 4. CONFIGURE BUS NOISE FILTERS AND SPEED TIMING PARAMETERS
     *------------------------------------------------------------------*/
    I2C1->CR1 &= ~I2C_CR1_PE; 

    I2C1->CR1 &= ~I2C_CR1_ANFOFF; 

    // Timing Register for 4 MHz clock source at 10 kHz speed
    I2C1->TIMINGR = (0x0U << 28)  | 
                    (0xC7U << 0)  | 
                    (0xC3U << 8)  | 
                    (0x02U << 16) | 
                    (0x04U << 20);  

    I2C1->CR1 |= I2C_CR1_PE;  
}

void i2c_transmit(uint8_t target_addr_7bit, uint8_t *data, uint8_t size) {
    while (I2C1->ISR & I2C_ISR_BUSY);

    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    
    // Explicitly mask and position address fields safely
    I2C1->CR2 |= (((uint32_t)target_addr_7bit << 1) & I2C_CR2_SADD); 
    I2C1->CR2 |= (((uint32_t)size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);          
    I2C1->CR2 |= I2C_CR2_AUTOEND; 
    I2C1->CR2 |= I2C_CR2_START;   

    for (uint8_t i = 0; i < size; i++) {
        // FIX: Prevent lockups by monitoring the NACK flag alongside TXIS
        while (!(I2C1->ISR & I2C_ISR_TXIS)) {
            if (I2C1->ISR & I2C_ISR_NACKF) {
                I2C1->ICR = I2C_ICR_NACKCF; // Clear NACK flag
                return; // Exit gracefully instead of deadlocking
            }
        }
        I2C1->TXDR = data[i]; 
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF; 
}

void i2c_receive(uint8_t target_addr_7bit, uint8_t *data, uint8_t size) {
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    
    I2C1->CR2 |= (((uint32_t)target_addr_7bit << 1) & I2C_CR2_SADD); 
    I2C1->CR2 |= (((uint32_t)size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);         
    I2C1->CR2 |= I2C_CR2_RD_WRN;                     
    I2C1->CR2 |= I2C_CR2_AUTOEND;             
    I2C1->CR2 |= I2C_CR2_START;    

    for (uint8_t i = 0; i < size; i++) {
        while (!(I2C1->ISR & I2C_ISR_RXNE)) {
            if (I2C1->ISR & I2C_ISR_NACKF) {
                I2C1->ICR = I2C_ICR_NACKCF;
                return;
            }
        }
        data[i] = I2C1->RXDR; 
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF; 
}

void i2c_read_mem(uint8_t addr, uint8_t reg, uint8_t nbytes, uint8_t *rx_buffer)
{
    while (I2C1->ISR & I2C_ISR_BUSY);

    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    
    // FIX: Force casting to 32-bit width to avoid compiler trimming optimization bugs
    I2C1->CR2 |= (((uint32_t)addr << 1) & I2C_CR2_SADD) | 
                 (((uint32_t)1 << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | 
                 I2C_CR2_START;
    
    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
        if (I2C1->ISR & I2C_ISR_NACKF) {
            I2C1->ICR = I2C_ICR_NACKCF;
            return;
        }
    }

    I2C1->TXDR = reg;          
    while (!(I2C1->ISR & I2C_ISR_TC)); // Wait until Transfer Complete (TC) flags

    // Read Phase (Harvest Data Elements over Repeated START)
    i2c_receive(addr, rx_buffer, nbytes);
}

void i2c_write_mem(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t nbytes) {
    while (I2C1->ISR & I2C_ISR_BUSY);

    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
    
    // Total size is: 1 byte (for register pointer) + data bytes
    uint32_t total_size = nbytes + 1;

    I2C1->CR2 |= (((uint32_t)addr << 1) & I2C_CR2_SADD); 
    I2C1->CR2 |= ((total_size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);          
    I2C1->CR2 |= I2C_CR2_AUTOEND; 
    I2C1->CR2 |= I2C_CR2_START;   

    // Byte 1: Transmit the target register location pointer
    while (!(I2C1->ISR & I2C_ISR_TXIS)) {
        if (I2C1->ISR & I2C_ISR_NACKF) { I2C1->ICR = I2C_ICR_NACKCF; return; }
    }
    I2C1->TXDR = reg;

    // Bytes 2+: Transmit the payload buffer elements
    for (uint8_t i = 0; i < nbytes; i++) {
        while (!(I2C1->ISR & I2C_ISR_TXIS)) {
            if (I2C1->ISR & I2C_ISR_NACKF) { I2C1->ICR = I2C_ICR_NACKCF; return; }
        }
        I2C1->TXDR = data[i]; 
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF; 
}
