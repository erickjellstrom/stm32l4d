
#include "stm32l4xx.h"
#include <stdint.h>
#include "i2c.h"

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

/**
 * @brief Phase 1: Begins an I2C transaction in Write mode.
 */
void proto_i2c_write_start(uint8_t dev_addr, uint8_t nbytes)
{
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Clear settings, ensure AUTOEND and RELOAD are turned off
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_RELOAD);
    
    I2C1->CR2 |= (((uint32_t)dev_addr << 1) & I2C_CR2_SADD); 
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; // Write mode
    I2C1->CR2 |= ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos);          

    I2C1->CR2 |= I2C_CR2_START;   
}

/**
 * @brief Phase 2: Sends data bytes and stretches SCL when finished.
 */
void proto_i2c_write_data(uint8_t *data, uint32_t nbytes)
{
    for (uint32_t i = 0; i < nbytes; i++) 
    {
        while (!(I2C1->ISR & I2C_ISR_TXIS)) {
            if (I2C1->ISR & I2C_ISR_NACKF) { I2C1->ICR = I2C_ICR_NACKCF; return; }
        }
        I2C1->TXDR = data[i];
    }

    // Wait until all specified bytes are sent. Hardware will now stretch SCL.
    while (!(I2C1->ISR & I2C_ISR_TC));
}

/**
 * @brief Phase 3: Generates a Repeated Start condition to switch to Read mode.
 */
void proto_i2c_repeated_start_read(uint8_t dev_addr, uint8_t nbytes)
{
    // Modify CR2 for reading (keep AUTOEND and RELOAD turned off)
    I2C1->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND | I2C_CR2_RELOAD);
    I2C1->CR2 |= (((uint32_t)dev_addr << 1) & I2C_CR2_SADD); 
    I2C1->CR2 |= I2C_CR2_RD_WRN;                               // Change to Read mode
    I2C1->CR2 |= ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos);    

    // Writing START while TC is active forces a RESTART
    I2C1->CR2 |= I2C_CR2_START;
}

/**
 * @brief Phase 4: Reads a single byte from the I2C receive buffer.
 */
uint8_t proto_i2c_read_byte(void)
{
    // Wait for the Receive Not Empty flag
    while (!(I2C1->ISR & I2C_ISR_RXNE)) {
        if (I2C1->ISR & I2C_ISR_NACKF) { I2C1->ICR = I2C_ICR_NACKCF; return 0; }
    }
    
    return (uint8_t)I2C1->RXDR;
}

/**
 * @brief Phase 5: Closes the transaction manually by generating a STOP condition.
 */
void proto_i2c_stop(void)
{
    // Wait for the final read transfer to complete completely
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Generate manual STOP
    I2C1->CR2 |= I2C_CR2_STOP;

    // Clear the STOP flag once acknowledged by hardware
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
}

void read_sensor_registers(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buffer, uint8_t length)
{
    // 1. Start write phase (expecting 1 byte of data: the register address)
    proto_i2c_write_start(dev_addr, 1);

    // 2. Write the register address (this will stretch the SCL line when done)
    proto_i2c_write_data(&reg_addr, 1);

    // 3. Issue Repeated Start and configure hardware to expect 'length' bytes
    proto_i2c_repeated_start_read(dev_addr, length);

    // 4. Cleanly read the bytes one by one at the application layer
    for (uint8_t i = 0; i < length; i++) {
        rx_buffer[i] = proto_i2c_read_byte();
    }

    // 5. Manually send the STOP condition to free the bus
    proto_i2c_stop();
}


void write_sensor_registers(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_addr_len, uint8_t *data, uint8_t data_len)
{
    uint8_t tx_buffer[10];
    for (int i=0; i<reg_addr_len; i++) {
        tx_buffer[i] = reg_addr;
    }
    for (int i=0; i<data_len; i++) {
        tx_buffer[reg_addr_len+i] = data[i];
    }
    
    // 1. Start write phase (expecting 1 byte of data: the register address)
    proto_i2c_write_start(dev_addr, reg_addr_len + data_len);

    // 2. Write the register address (this will stretch the SCL line when done)
    proto_i2c_write_data(tx_buffer, reg_addr_len + data_len);
    
    // 5. Manually send the STOP condition to free the bus
    proto_i2c_stop();
}
