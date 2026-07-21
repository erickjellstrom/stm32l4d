#include "error_statemachine.h"
#include "app.h"


static void do_no_error(void) {printf("do_no_error(void)\n");}
static void do_ext_error(void) {printf("do_ext_error(void)\n");}
static void do_int_error(void) {printf("do_int_error(void)\n");}
static void do_int_failure(void) {printf("do_int_failure(void)\n");}
static void do_int_perm_failure(void) {printf("do_perm_int_failure(void)\n");} 

const struct error_statemachine error_sm[STATE_ERROR_COUNT] = {
    [STATE_NO_ERROR] = {
        .error_state = STATE_NO_ERROR, 
        .action = do_no_error,
        .next = {
            [INPUT_NO_ERROR]  = &error_sm[STATE_NO_ERROR], 
            [INPUT_EXT_ERROR]  = &error_sm[STATE_EXT_ERROR], 
            [INPUT_INT_ERROR] = &error_sm[STATE_INT_ERROR], 
            [INPUT_INT_FAILURE]  = &error_sm[STATE_INT_FAILURE],
            [INPUT_INT_PERM_FAILURE]  = &error_sm[STATE_INT_PERM_FAILURE]
        }
    },
    [STATE_EXT_ERROR] = {
        .error_state = STATE_EXT_ERROR, 
        .action = do_ext_error,
        .next = {
            [INPUT_NO_ERROR]  = &error_sm[STATE_NO_ERROR], 
            [INPUT_EXT_ERROR]  = &error_sm[STATE_EXT_ERROR], 
            [INPUT_INT_ERROR] = &error_sm[STATE_INT_ERROR], 
            [INPUT_INT_FAILURE]  = &error_sm[STATE_INT_FAILURE],
            [INPUT_INT_PERM_FAILURE]  = &error_sm[STATE_INT_PERM_FAILURE]
        }
    },
    [STATE_INT_ERROR] = {
        .error_state = STATE_INT_ERROR, 
        .action = do_int_error,
        .next = {
            [INPUT_NO_ERROR]  = &error_sm[STATE_NO_ERROR], 
            [INPUT_EXT_ERROR]  = &error_sm[STATE_EXT_ERROR], 
            [INPUT_INT_ERROR] = &error_sm[STATE_INT_ERROR], 
            [INPUT_INT_FAILURE]  = &error_sm[STATE_INT_FAILURE],
            [INPUT_INT_PERM_FAILURE]  = &error_sm[STATE_INT_PERM_FAILURE]
        }
    },
    [STATE_INT_FAILURE] = {
        .error_state = STATE_INT_FAILURE, 
        .action = do_int_failure,
        .next = {
            [INPUT_NO_ERROR]  = &error_sm[STATE_NO_ERROR], 
            [INPUT_EXT_ERROR]  = &error_sm[STATE_EXT_ERROR], 
            [INPUT_INT_ERROR] = &error_sm[STATE_INT_ERROR], 
            [INPUT_INT_FAILURE]  = &error_sm[STATE_INT_FAILURE],
            [INPUT_INT_PERM_FAILURE]  = &error_sm[STATE_INT_PERM_FAILURE]
        }
    },
    [STATE_INT_PERM_FAILURE] = {
        .error_state = STATE_INT_PERM_FAILURE, 
        .action = do_int_perm_failure,
        .next = {
            [INPUT_NO_ERROR]  = &error_sm[STATE_NO_ERROR], 
            [INPUT_EXT_ERROR]  = &error_sm[STATE_EXT_ERROR], 
            [INPUT_INT_ERROR] = &error_sm[STATE_INT_ERROR], 
            [INPUT_INT_FAILURE]  = &error_sm[STATE_INT_FAILURE],
            [INPUT_INT_PERM_FAILURE]  = &error_sm[STATE_INT_PERM_FAILURE]
        }
    }
};

// Initializes the state machine context safely
void error_sm_init(struct error_statemachine** error_sm_ref, error_state_t initial_state) {
    if (initial_state >= STATE_ERROR_COUNT) return;
    *error_sm_ref = &error_sm[initial_state];
    
}

void error_sm_execute(struct error_statemachine* error_sm)
{
    error_sm->action();
}


// Moves to the next state based on the input event and runs its loop action
bool error_sm_process_event(struct error_statemachine** error_sm_ref, error_input_t event) {
    if (event >= INPUT_COUNT) return false;

    struct error_statemachine* next_state = (*error_sm_ref)->next[event];
    
    if (next_state == NULL) return false; // Guard against unmapped paths

    bool state_changed = (next_state != *error_sm_ref);
    
    // Perform transition
    *error_sm_ref = next_state;

    return state_changed;
}