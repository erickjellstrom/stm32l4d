#include "rtc.h"
#include "i2c.h"

// Global time parameters optimized for VS Code Cortex-Debug Live Watch
volatile uint8_t hours = 0;
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;


void set_rtc_time(uint8_t sec, uint8_t min, uint8_t hour) {
    uint8_t time_buffer[3];

    time_buffer[0] = DEC_TO_BCD(sec);   
    time_buffer[1] = DEC_TO_BCD(min);  
    time_buffer[2] = DEC_TO_BCD(hour);  

    // Target PCF8563 (0x51), start at Seconds register (0x02), transmit 3 bytes
    write_sensor_registers(PCF8563_7BIT_ADDR, 0x02, 1, time_buffer, 3);
}

void get_rtc_time(uint8_t *time) {
    uint8_t raw_buffer[3];
    
    // Read 3 bytes starting at the Seconds register (0x02)
    
    read_sensor_registers(PCF8563_7BIT_ADDR, 0x02, raw_buffer, 3);
    // NOTE: PCF8563 uses the highest bit of the seconds register as a corruption flag.
    // Masking with 0x7F isolates just the BCD seconds data before converting.
    time[0] = BCD_TO_DEC(raw_buffer[0] & 0x7F);
    time[1] = BCD_TO_DEC(raw_buffer[1] & 0x7F);
    time[2] = BCD_TO_DEC(raw_buffer[2] & 0x3F); // Hours register uses lower 6 bits

}