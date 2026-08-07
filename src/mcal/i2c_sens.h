#include "stm32l4xx.h"

void i2c_sens_init(void);
void i2c_sens_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buffer, uint8_t length);
void i2c_sens_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_addr_len, uint8_t *data, uint8_t data_len);
//uint8_t i2c_is_device_ready(I2C_TypeDef *i2cx, uint8_t dev_addr, uint32_t trials);