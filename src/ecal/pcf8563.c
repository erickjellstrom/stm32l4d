#include "rtc.h"
#include "i2c.h"
#include "defines.h"

#define PCF8563_7BIT_ADDR    0x51  
#define REG_VL_SECONDS       0x02
#define REG_VL_MINUTES       0x03
#define REG_VL_HOURS         0x04



void rtc_set_time(uint8_t sec, uint8_t min, uint8_t hour) {
    uint8_t time_buffer[3];

    time_buffer[0] = DEC_TO_BCD(sec);   
    time_buffer[1] = DEC_TO_BCD(min);  
    time_buffer[2] = DEC_TO_BCD(hour);  

    // Target PCF8563 (0x51), start at Seconds register (0x02), transmit 3 bytes
    i2c_write_reg(PCF8563_7BIT_ADDR, 0x02, 1, time_buffer, 3);
}

void rtc_get_time(uint8_t *time) {
    uint8_t raw_buffer[3];
    
    // Read 3 bytes starting at the Seconds register (0x02)
    
    i2c_read_reg(PCF8563_7BIT_ADDR, 0x02, raw_buffer, 3);
    //read_sensor_registers(PCF8563_7BIT_ADDR, 0x02, raw_buffer, 3);
    // NOTE: PCF8563 uses the highest bit of the seconds register as a corruption flag.
    // Masking with 0x7F isolates just the BCD seconds data before converting.
    time[0] = BCD_TO_DEC(raw_buffer[0] & 0x7F);
    time[1] = BCD_TO_DEC(raw_buffer[1] & 0x7F);
    time[2] = BCD_TO_DEC(raw_buffer[2] & 0x3F); // Hours register uses lower 6 bits

}