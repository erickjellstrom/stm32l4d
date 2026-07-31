
#include "data.h"
#include "gpio.h"

void sensor_loop(void)
{
    // set global variable d2 with button status
    gpio_d2 = gpio_d2_get();


    // Fetch raw 12-bit sample (0 - 4095)
    adc_raw_value = adc_read();
    
    // Translate digital format back to an absolute voltage range (assumes VREF = 3.3V)
    adc_input_voltage = ((float)adc_raw_value * 3.3f) / 4095.0f;

}

void sensor_button_init(void)
{
    gpio_button_init();
    irq_button_init();
}