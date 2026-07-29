#ifndef APP_STATEMACHINE_H  
#define APP_STATEMACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    STATE_INIT = 0,
    STATE_STANDBY = 1,
    STATE_RUNNING = 2,
    STATE_ERROR = 3,
    STATE_COUNT // Automatically tracks total number of states (4)
} app_state_t;

struct app_statemachine_state {
    app_state_t state;
    struct app_statemachine_state* next[3];
    void (*action)(void); // Function pointer
};

extern struct app_statemachine_state* app_sm;


typedef enum {
    INPUT_STANDBY = 0,    // Replaces raw integer 0
    INPUT_RUN = 1,   // Replaces raw integer 1
    INPUT_ERROR = 2,    // Replaces raw integer 2
    INPUT_COUNT        // Automatically tracks total inputs (3)
} app_input_t;


// Core Engine Functions
void sm_init(struct app_statemachine_state** sm_ref, app_state_t initial_state);
bool sm_process_event(struct app_statemachine_state** sm, app_input_t event);

#endif //APP_STATEMACHINE_H