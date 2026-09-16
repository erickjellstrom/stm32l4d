#include "data.h"

//rtc
volatile uint8_t rtc_time[3]; 

//adc
volatile uint32_t adc_raw_value;
volatile float adc_input_voltage;

//gpio
volatile uint8_t gpio_d2;

volatile uint8_t g_start = 0;

//imu
float temperature_celsius;
int16_t raw_x;
int16_t raw_y;
int16_t raw_z;
int16_t raw_t;

uint8_t accel_buffer[6] = {0};
uint8_t temp_buffer[2] = {0};

float accel_x_mg;
float accel_y_mg;
float accel_z_mg;