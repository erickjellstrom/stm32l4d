

#include "stm32l4xx.h"  // CMSIS Core Device Header

void i2c_init(void);
void i2c_transmit(uint8_t target_addr_7bit, uint8_t *data, uint8_t size);
void i2c_receive(uint8_t target_addr_7bit, uint8_t *data, uint8_t size);
void i2c_read_mem(uint8_t addr, uint8_t reg, uint8_t nbytes, uint8_t *rx_buffer);
void i2c_write_mem(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t nbytes);