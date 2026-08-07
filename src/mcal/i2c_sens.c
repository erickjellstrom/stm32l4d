#include "stm32l4xx.h"
#include <stdint.h>
#include "i2c_sens.h"

void i2c_sens_init(void) {
    /*------------------------------------------------------------------
     * 1. ENABLE CLOCKS (GPIOB and I2C2)
     *------------------------------------------------------------------*/
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;   // Supply power clock to GPIOB
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;  // Supply power clock to I2C2

    /*------------------------------------------------------------------
     * 2. RESET I2C2 PERIPHERAL TO CLEAN STATE
     *------------------------------------------------------------------*/
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C2RST;  // Assert reset line
    for(volatile int i = 0; i < 200; i++);     // Wait for stabilization
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C2RST; // De-assert reset line

    /*------------------------------------------------------------------
     * 3. CONFIGURE INTERFACE PINS (PB10 = SCL, PB11 = SDA)
     *------------------------------------------------------------------*/
    // Clear and set Alternate Function Mode (2U) for PB10 and PB11
    GPIOB->MODER &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
    GPIOB->MODER |=  ((2U << (10 * 2)) | (2U << (11 * 2)));

    // Set open-drain flags for output types
    GPIOB->OTYPER |= ((1U << 10) | (1U << 11)); 

    // Clear pull-up/pull-down register (No Pull)
    GPIOB->PUPDR &= ~((3U << (10 * 2)) | (3U << (11 * 2)));

    // Configure Alternate Function Register High (AFR[1]) to AF4 (I2C2)
    GPIOB->AFR[1] &= ~((0xFU << ((10 - 8) * 4)) | (0xFU << ((11 - 8) * 4)));
    GPIOB->AFR[1] |=  ((4U << ((10 - 8) * 4))   | (4U << ((11 - 8) * 4)));

    /*------------------------------------------------------------------
     * 4. CONFIGURE BUS NOISE FILTERS AND SPEED TIMING PARAMETERS
     *------------------------------------------------------------------*/
    I2C2->CR1 &= ~I2C_CR1_PE; // Disable peripheral before writing configuration

    I2C2->CR1 &= ~I2C_CR1_ANFOFF; // Enable Analog Noise Filter

    // Timing Register for 4 MHz clock source at 10 kHz speed (matches your I2C1 template)
    I2C2->TIMINGR = (0x0U << 28)  | 
                    (0xC7U << 0)  | 
                    (0xC3U << 8)  | 
                    (0x02U << 16) | 
                    (0x04U << 20);  

    I2C2->CR1 |= I2C_CR1_PE;  // Re-enable peripheral
}



static void i2c_write(uint8_t dev_addr, uint8_t *data ,uint8_t nbytes)
{

    while (I2C2->ISR & I2C_ISR_BUSY);

    // Clear settings, ensure AUTOEND and RELOAD are turned off
    I2C2->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_RELOAD);
    
    I2C2->CR2 |= (((uint32_t)dev_addr << 1) & I2C_CR2_SADD); 
    I2C2->CR2 &= ~I2C_CR2_RD_WRN; // Write mode
    I2C2->CR2 |= ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos);          

    I2C2->CR2 |= I2C_CR2_START;   
    
    for (uint32_t i = 0; i < nbytes; i++) 
    {
        while (!(I2C2->ISR & I2C_ISR_TXIS)) {
            if (I2C2->ISR & I2C_ISR_NACKF) { I2C2->ICR = I2C_ICR_NACKCF; return; }
        }
        I2C2->TXDR = data[i];
    }

    // Wait until all specified bytes are sent. Hardware will now stretch SCL.
    while (!(I2C2->ISR & I2C_ISR_TC));
}

static void i2c_stop(void)
{
    // Wait for the final read transfer to complete completely
    while (!(I2C2->ISR & I2C_ISR_TC));

    // Generate manual STOP
    I2C2->CR2 |= I2C_CR2_STOP;

    // Clear the STOP flag once acknowledged by hardware
    while (!(I2C2->ISR & I2C_ISR_STOPF));
    I2C2->ICR = I2C_ICR_STOPCF;
}


static void i2c_repeated_start_read(uint8_t dev_addr, uint8_t *rx_buffer, uint8_t nbytes)
{
    // Modify CR2 for reading (keep AUTOEND and RELOAD turned off)
    I2C2->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_RELOAD);
    I2C2->CR2 |= (((uint32_t)dev_addr << 1) & I2C_CR2_SADD); 
    I2C2->CR2 |= I2C_CR2_RD_WRN;                               // Change to Read mode
    I2C2->CR2 |= ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos);    

    // Writing START while TC is active forces a RESTART
    I2C2->CR2 |= I2C_CR2_START;

    for (uint8_t i = 0; i < nbytes; i++) {
        // Wait for the Receive Not Empty flag
        while (!(I2C2->ISR & I2C_ISR_RXNE)) {
        if (I2C2->ISR & I2C_ISR_NACKF) { I2C2->ICR = I2C_ICR_NACKCF; return 0; }
        }
        rx_buffer[i] = (uint8_t)I2C2->RXDR;
    }
    
}

void i2c_sens_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buffer, uint8_t length)
{
    i2c_write(dev_addr, &reg_addr , 1);

    i2c_repeated_start_read(dev_addr,rx_buffer, length);

    i2c_stop();
}

void i2c_sens_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_addr_len, uint8_t *data, uint8_t data_len)
{
    uint8_t tx_buffer[10];
    for (int i=0; i<reg_addr_len; i++) {
        tx_buffer[i] = reg_addr;
    }
    for (int i=0; i<data_len; i++) {
        tx_buffer[reg_addr_len+i] = data[i];
    }

    i2c_write(dev_addr, tx_buffer, reg_addr_len + data_len);
    
    i2c_stop();
}

/**
 * @brief  Checks if target I2C device is ready for communication.
 * @param  i2cx: Pointer to I2C peripheral register structure (e.g., I2C1, I2C2).
 * @param  dev_addr: 7-bit device address (without read/write bit shifted).
 * @param  trials: Number of transmission attempts.
 * @retval true if device responds with ACK, false if NACK or timeout occurs.
 */
/*
uint8_t i2c_is_device_ready(I2C_TypeDef *i2cx, uint8_t dev_addr, uint32_t trials) {
    uint32_t timeout;
    
    for (uint32_t i = 0; i < trials; i++) {
        // 1. Clear CR2 register configuration for the transfer
        i2cx->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);
        
        // 2. Configure: 7-bit address, 0 bytes to transmit (checking ACK only), Write operation, Auto-End
        i2cx->CR2 |= ((uint32_t)dev_addr << 1) & I2C_CR2_SADD;
        i2cx->CR2 |= I2C_CR2_AUTOEND; 
        
        // 3. Generate START condition
        i2cx->CR2 |= I2C_CR2_START;
        
        // 4. Wait for STOP flag (AUTOEND handles this) or NACK flag
        timeout = 100000; // Software timeout loop counter
        while (!(i2cx->ISR & (I2C_ISR_STOPF | I2C_ISR_NACKF)) && timeout) {
            timeout--;
        }
        
        // 5. Check if NACK occurred
        if (i2cx->ISR & I2C_ISR_NACKF) {
            i2cx->ICR = I2C_ICR_NACKCF; // Clear NACK flag
            i2cx->ICR = I2C_ICR_STOPCF; // Clear STOP flag
            continue;                   // Try next trial
        }
        
        // 6. If STOP is detected without NACK, device ACKed successfully
        if (i2cx->ISR & I2C_ISR_STOPF) {
            i2cx->ICR = I2C_ICR_STOPCF; // Clear STOP flag
            return 1;                // Device is ready
        }
    }
    
    return 0; // Device did not respond after all trials
}
*/
