#ifndef APP_H
#define APP_H

#include "app_statemachine.h"

extern volatile uint8_t g_start;

void app_init();
void app_standby();
void app_run();
void app_error();
void app_failures();

void log_sensor();

#endif //APP_H