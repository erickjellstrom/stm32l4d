#ifndef DATA_H
#define DATA_H

#include <stdint.h>

//adc
extern volatile uint8_t rtc_time[3]; 

//rtc
extern volatile uint32_t adc_raw_value;
extern volatile float adc_input_voltage;

//gpio
extern volatile uint8_t gpio_d2;

extern volatile uint8_t g_start;

//imu
extern float temperature_celsius;
extern int16_t raw_x;
extern int16_t raw_y;
extern int16_t raw_z;
extern int16_t raw_t;

extern uint8_t accel_buffer[];
extern uint8_t temp_buffer[];

extern float accel_x_mg;
extern float accel_y_mg;
extern float accel_z_mg;

#endif //DATA_H