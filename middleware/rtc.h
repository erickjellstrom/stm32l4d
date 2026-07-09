#include <stdint.h>
#include "i2c.h"

#define PCF8563_7BIT_ADDR    0x51  
#define REG_VL_SECONDS       0x02
#define REG_VL_MINUTES       0x03
#define REG_VL_HOURS         0x04

#define DEC_TO_BCD(val)   (( (val / 10) << 4) | (val % 10) )
#define BCD_TO_DEC(val)   (( ((val) >> 4) * 10 ) + ((val) & 0x0F))

void set_rtc_time(uint8_t sec, uint8_t min, uint8_t hour);
void get_rtc_time(uint8_t *time);