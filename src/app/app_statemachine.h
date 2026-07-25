#ifndef APP_STATEMACHINE_H  
#define APP_STATEMACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    STATE_IDLE = 0,
    STATE_STANDBY = 1,
    STATE_RUNNING = 2,
    STATE_ERROR = 3,
    STATE_COUNT // Automatically tracks total number of states (4)
} app_state_t;

typedef enum {
    INPUT_STOP = 0,    // Replaces raw integer 0
    INPUT_START = 1,   // Replaces raw integer 1
    INPUT_FAIL = 2,    // Replaces raw integer 2
    INPUT_COUNT        // Automatically tracks total inputs (3)
} app_input_t;

struct app_statemachine {
    app_state_t state;
    struct app_statemachine* next[3];
    void (*action)(void); // Function pointer
};

// Core Engine Functions
void sm_init(struct app_statemachine** sm_ref, app_state_t initial_state);
bool sm_process_event(struct app_statemachine** sm, app_input_t event);
void sm_execute(struct app_statemachine* sm);

#endif //APP_STATEMACHINE_H