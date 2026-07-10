#include "stm32l4xx.h"

void i2c_init(void);
void i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buffer, uint8_t length);
void i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_addr_len, uint8_t *data, uint8_t data_len);