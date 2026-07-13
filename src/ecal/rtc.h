#include <stdint.h>
#include "i2c.h"

void rtc_set_time(uint8_t sec, uint8_t min, uint8_t hour);
void rtc_get_time(uint8_t *time);