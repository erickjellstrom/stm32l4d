#ifndef ERROR_STATEMACHINE_H  
#define ERROR_STATEMACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    STATE_NO_ERROR = 0,
    STATE_EXT_ERROR = 1,
    STATE_INT_ERROR = 2,
    STATE_INT_FAILURE = 3,
    STATE_INT_PERM_FAILURE = 4,
    STATE_ERROR_COUNT // Automatically tracks total number of states (5)
} error_state_t;

typedef enum {
    INPUT_NO_ERROR = 0,    // Replaces raw integer 0
    INPUT_EXT_ERROR = 1,    // Replaces raw integer 0
    INPUT_INT_ERROR = 2,   // Replaces raw integer 1
    INPUT_INT_FAILURE = 3,    // Replaces raw integer 2
    INPUT_INT_PERM_FAILURE = 4,    // Replaces raw integer 3
    INPUT_ERROR_COUNT        // Automatically tracks total inputs (5)
} error_input_t;

struct error_statemachine {
    error_state_t error_state;
    struct error_statemachine* next[5];
    void (*action)(void); // Function pointer
};

// Core Engine Functions
void error_sm_init(struct error_statemachine** error_sm_ref, error_state_t initial_state);
bool error_sm_process_event(struct error_statemachine** error_sm, error_input_t event);
void error_sm_execute(struct error_statemachine* error_sm);

#endif //ERROR_STATEMACHINE_H