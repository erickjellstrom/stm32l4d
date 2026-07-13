#ifndef STATEMACHINE_H  
#define STATEMACHINE_H


// Define named states using an enum
typedef enum {
    STATE_IDLE = 0,
    STATE_RUNNING = 1,
    STATE_ERROR = 2,
    STATE_COUNT // Automatically tracks total number of states (3)
} state_t;


struct statemachine {
    uint8_t state;
    struct statemachine* next[3];
};

#endif //STATEMACHINE_H