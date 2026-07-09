


#include "stm32l4xx.h"
void i2c_init(void);

void proto_i2c_write_start(uint8_t dev_addr, uint8_t nbytes);
void proto_i2c_write_data(uint8_t *data, uint32_t nbytes);
void proto_i2c_repeated_start_read(uint8_t dev_addr, uint8_t nbytes);
uint8_t proto_i2c_read_byte(void);
void proto_i2c_stop(void);
void read_sensor_registers(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_buffer, uint8_t length);
void write_sensor_registers(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_addr_len, uint8_t *data, uint8_t data_len);
