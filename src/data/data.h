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

//console
extern uint8_t console_cmd;

//extern volatile uint8_t g_my_test_var;

typedef struct {
    volatile uint8_t digital_switch_1;
    volatile uint8_t digital_switch_2;
    volatile uint16_t analog_slider_val;
    volatile uint8_t state_machine_cmd;
} PythonInputs_t;

extern volatile PythonInputs_t g_board_inputs;

//extern volatile uint32_t another_p;

//extern volatile uint16_t my_variable;
#endif //DATA_H