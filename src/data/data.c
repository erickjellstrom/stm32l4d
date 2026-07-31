#include "data.h"

//rtc
volatile uint8_t rtc_time[3]; 

//adc
volatile uint32_t adc_raw_value;
volatile float adc_input_voltage;

//gpio
volatile uint8_t gpio_d2;

volatile uint8_t g_start = 0;