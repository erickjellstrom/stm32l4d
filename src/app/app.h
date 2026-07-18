#ifndef APP_H
#define APP_H

#include "statemachine.h"

extern volatile uint8_t g_start;

void app_init();
void app_standby();
void app_run();
void app_error();
void app_failures();
input_t app_input();

#endif //APP_H