#include "app_statemachine.h"
#include "app.h"

extern main_init();

static void do_idle(void) { app_init(); }
static void do_standby(void) { app_standby(); }
static void do_running(void) { app_run(); }
static void do_error(void)   { app_error(); } 

const struct app_statemachine sm[STATE_COUNT] = {
    [STATE_IDLE] = {
        .state = STATE_IDLE, 
        .action = do_idle,
        .next = {
            [INPUT_STOP]  = &sm[STATE_STANDBY], 
            [INPUT_START] = &sm[STATE_RUNNING], 
            [INPUT_FAIL]  = &sm[STATE_STANDBY]
        }
    },
    [STATE_STANDBY] = {
        .state = STATE_STANDBY, 
        .action = do_standby,
        .next = {
            [INPUT_STOP]  = &sm[STATE_STANDBY], 
            [INPUT_START] = &sm[STATE_RUNNING], 
            [INPUT_FAIL]  = &sm[STATE_ERROR]
        }
    },
    [STATE_RUNNING] = {
        .state = STATE_RUNNING, 
        .action = do_running,
        .next = {
            [INPUT_STOP]  = &sm[STATE_STANDBY], 
            [INPUT_START] = &sm[STATE_RUNNING], 
            [INPUT_FAIL]  = &sm[STATE_ERROR]
        }
    },
    [STATE_ERROR] = {
        .state = STATE_ERROR, 
        .action = do_error,
        .next = {
            [INPUT_STOP]  = &sm[STATE_STANDBY], 
            [INPUT_START] = &sm[STATE_RUNNING],   
            [INPUT_FAIL]  = &sm[STATE_ERROR]
        }
    }
};

// Initializes the state machine context safely
void sm_init(struct app_statemachine** sm_ref, app_state_t initial_state) {
    if (initial_state >= STATE_COUNT) return;
    *sm_ref = &sm[initial_state];
    
}

void sm_execute(struct app_statemachine* sm)
{
    sm->action();
}


// Moves to the next state based on the input event and runs its loop action
bool sm_process_event(struct app_statemachine** sm_ref, app_input_t event) {
    if (event >= INPUT_COUNT) return false;

    struct app_statemachine* next_state = (*sm_ref)->next[event];
    
    if (next_state == NULL) return false; // Guard against unmapped paths

    bool state_changed = (next_state != *sm_ref);
    
    // Perform transition
    *sm_ref = next_state;

    return state_changed;
}