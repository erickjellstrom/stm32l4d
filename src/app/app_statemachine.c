#include "app_statemachine.h"
#include "app_handler.h"


static void do_init(void) { app_init(); }
static void do_standby(void) { app_standby(); }
static void do_running(void) { app_run(); }
static void do_error(void)   { app_error(); } 

struct app_statemachine_state* app_sm;

const struct app_statemachine_state app_statemachine[STATE_COUNT] = {
    [STATE_INIT] = {
        .state = STATE_INIT, 
        .action = do_init,
        .next = {
            [INPUT_STANDBY]  = &app_statemachine[STATE_STANDBY], 
            [INPUT_RUN] = &app_statemachine[STATE_RUNNING], 
            [INPUT_ERROR]  = &app_statemachine[STATE_ERROR]
        }
    },
    [STATE_STANDBY] = {
        .state = STATE_STANDBY, 
        .action = do_standby,
        .next = {
            [INPUT_STANDBY]  = &app_statemachine[STATE_STANDBY], 
            [INPUT_RUN] = &app_statemachine[STATE_RUNNING], 
            [INPUT_ERROR]  = &app_statemachine[STATE_ERROR]
        }
    },
    [STATE_RUNNING] = {
        .state = STATE_RUNNING, 
        .action = do_running,
        .next = {
            [INPUT_STANDBY]  = &app_statemachine[STATE_STANDBY], 
            [INPUT_RUN] = &app_statemachine[STATE_RUNNING], 
            [INPUT_ERROR]  = &app_statemachine[STATE_ERROR]
        }
    },
    [STATE_ERROR] = {
        .state = STATE_ERROR, 
        .action = do_error,
        .next = {
            [INPUT_STANDBY]  = &app_statemachine[STATE_STANDBY], 
            [INPUT_RUN] = &app_statemachine[STATE_RUNNING],   
            [INPUT_ERROR]  = &app_statemachine[STATE_ERROR]
        }
    }
};

// Initializes the state machine context safely
void sm_init(struct app_statemachine_state** sm_ref, app_state_t initial_state) {
    if (initial_state >= STATE_COUNT) return;
    *sm_ref = &app_statemachine[initial_state];
    
    // Execute state
    (*sm_ref)->action();
}

// Moves to the next state based on the input event and runs its loop action
bool sm_process_event(struct app_statemachine_state** sm_ref, app_input_t event) {
    if (event >= INPUT_COUNT) return false;

    struct app_statemachine_state* next_state = (*sm_ref)->next[event];
    
    if (next_state == NULL) return false; // Guard against unmapped paths

    bool state_changed = (next_state != *sm_ref);
    
    // Perform transition
    *sm_ref = next_state;

    // Execute state
    (*sm_ref)->action();

    return state_changed;
}