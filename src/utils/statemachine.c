#include <stdint.h>
#include "statemachine.h"
/*
const struct statemachine app_sm[] = {
    {STATE_IDLE, {&app_sm[STATE_IDLE], &app_sm[STATE_RUNNING], &app_sm[STATE_IDLE]}}, // State 0
    {STATE_RUNNING, {&app_sm[STATE_IDLE], &app_sm[STATE_RUNNING], &app_sm[STATE_ERROR]}}, // State 1
    {STATE_ERROR, {&app_sm[STATE_IDLE], &app_sm[STATE_ERROR], &app_sm[STATE_ERROR]}}  // State 2
};
*/                                   

const struct statemachine app_sm[] = {
    [STATE_IDLE] = {
        .state = STATE_IDLE, 
        .next = {&app_sm[STATE_IDLE], &app_sm[STATE_RUNNING], &app_sm[STATE_IDLE]}
    },
    [STATE_RUNNING] = {
        .state = STATE_RUNNING, 
        .next = {&app_sm[STATE_IDLE], &app_sm[STATE_RUNNING], &app_sm[STATE_ERROR]}
    },
    [STATE_ERROR] = {
        .state = STATE_ERROR, 
        .next = {&app_sm[STATE_IDLE], &app_sm[STATE_ERROR],   &app_sm[STATE_ERROR]}
    }
};