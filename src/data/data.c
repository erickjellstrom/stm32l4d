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

// Variable controled by uart interface
uint8_t console_cmd = 1;

//volatile uint8_t g_my_test_var = 0;

// Variable controlled by gdb interface
__attribute__((section(".my_section"), used)) 
volatile PythonInputs_t g_board_inputs = {
    .digital_switch_1  = 0,      // Initialized to 1
    .digital_switch_2  = 1,      // Initialized to 0
    .analog_slider_val = 3000,   // Initialized to 1500 (fits in uint16_t)
    .state_machine_cmd = 2       // Initialized to 2
};


//volatile uint32_t another_p = 75;

//__attribute__((used)) uint16_t my_variable = 0;
//__attribute__((section(".my_section"), used)) uint16_t my_variable = 0;

//__attribute__((section(".my_section"), used)) volatile uint16_t my_variable = 0;


