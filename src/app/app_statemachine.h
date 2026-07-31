#ifndef APP_STATEMACHINE_H  
#define APP_STATEMACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    APP_STATE_INIT = 0,
    APP_STATE_STANDBY = 1,
    APP_STATE_RUNNING = 2,
    APP_STATE_ERROR = 3,
    APP_STATE_COUNT // Automatically tracks total number of states (4)
} app_state_t;

struct app_statemachine_state {
    app_state_t state;
    struct app_statemachine_state* next[3];
    void (*action)(void); // Function pointer
};

extern struct app_statemachine_state* app_sm;


typedef enum {
    APP_CMD_STANDBY = 0,    // Replaces raw integer 0
    APP_CMD_RUN = 1,   // Replaces raw integer 1
    APP_CMD_ERROR = 2,    // Replaces raw integer 2
    APP_CMD_COUNT        // Automatically tracks total inputs (3)
} app_cmd_t;


// Core Engine Functions
void sm_init(struct app_statemachine_state** sm_ref, app_state_t initial_state);
bool sm_process_event(struct app_statemachine_state** sm, app_cmd_t event);

#endif //APP_STATEMACHINE_H