#ifndef DATA_H
#define DATA_H

#include <stdint.h>

//adc
extern volatile uint8_t rtc_time[3]; 

//rtc
extern volatile uint32_t adc_raw_value;
extern volatile float adc_input_voltage;

#endif //DATA_H