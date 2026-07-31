#include "app_statemachine.h"
#include "app_handler.h"

struct app_statemachine_state* app_sm;

const struct app_statemachine_state app_statemachine[APP_STATE_COUNT] = {
    [APP_STATE_INIT] = {
        .state = APP_STATE_INIT, 
        .action = app_init,
        .next = {
            [APP_CMD_STANDBY]  = &app_statemachine[APP_STATE_STANDBY], 
            [APP_CMD_RUN] = &app_statemachine[APP_STATE_RUNNING], 
            [APP_CMD_ERROR]  = &app_statemachine[APP_STATE_ERROR]
        }
    },
    [APP_STATE_STANDBY] = {
        .state = APP_STATE_STANDBY, 
        .action = app_standby,
        .next = {
            [APP_CMD_STANDBY]  = &app_statemachine[APP_STATE_STANDBY], 
            [APP_CMD_RUN] = &app_statemachine[APP_STATE_RUNNING], 
            [APP_CMD_ERROR]  = &app_statemachine[APP_STATE_ERROR]
        }
    },
    [APP_STATE_RUNNING] = {
        .state = APP_STATE_RUNNING, 
        .action = app_run,
        .next = {
            [APP_CMD_STANDBY]  = &app_statemachine[APP_STATE_STANDBY], 
            [APP_CMD_RUN] = &app_statemachine[APP_STATE_RUNNING], 
            [APP_CMD_ERROR]  = &app_statemachine[APP_STATE_ERROR]
        }
    },
    [APP_STATE_ERROR] = {
        .state = APP_STATE_ERROR, 
        .action = app_error,
        .next = {
            [APP_CMD_STANDBY]  = &app_statemachine[APP_STATE_STANDBY], 
            [APP_CMD_RUN] = &app_statemachine[APP_STATE_RUNNING],   
            [APP_CMD_ERROR]  = &app_statemachine[APP_STATE_ERROR]
        }
    }
};

// Initializes the state machine context safely
void sm_init(struct app_statemachine_state** sm_ref, app_state_t initial_state) {
    if (initial_state >= APP_STATE_COUNT) return;
    *sm_ref = &app_statemachine[initial_state];
    
    // Execute state
    (*sm_ref)->action();
}

// Moves to the next state based on the input event and runs its loop action
bool sm_process_event(struct app_statemachine_state** sm_ref, app_cmd_t event) {
    if (event >= APP_CMD_COUNT) return false;

    struct app_statemachine_state* next_state = (*sm_ref)->next[event];
    
    if (next_state == NULL) return false; // Guard against unmapped paths

    bool state_changed = (next_state != *sm_ref);
    
    // Perform transition
    *sm_ref = next_state;

    // Execute state
    (*sm_ref)->action();

    return state_changed;
}