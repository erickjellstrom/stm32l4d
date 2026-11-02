#include "tx_api.h"
#include "main.h"
#include "threads.h"

// Define properties for our test thread
#define THREAD_STACK_SIZE 1024
TX_THREAD thread_msg;
uint8_t thread_stack[THREAD_STACK_SIZE];


void tx_application_define(void *first_unused_memory)
{
    tx_thread_create(
        &thread_msg,
        "Blinky Thread",
        thread_msg_entry,
        0,
        thread_stack,
        THREAD_STACK_SIZE,
        10,            // Priority level (Lower number = higher priority)
        10,            // Preemption threshold
        TX_NO_TIME_SLICE,
        TX_AUTO_START  // Start running immediately
    );
}
