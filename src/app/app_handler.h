#ifndef APP_H
#define APP_H

#include "app_statemachine.h"

extern volatile uint8_t g_start;

extern struct app_statemachine* app_sm;
extern app_input_t app_inp;

void app_init();
void app_standby();
void app_run();
void app_error();
void app_failures();
//app_input_t app_input();

#endif //APP_H