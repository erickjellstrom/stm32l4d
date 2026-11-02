

#include "tx_api.h"
#include "main.h"
#include "threads.h"

// Define properties for our test thread
#define THREAD_COUNT 5
#define THREAD_STACK_SIZE 1024
static TX_THREAD thread_pfr[THREAD_COUNT];
static ULONG thread_stack[THREAD_COUNT][THREAD_STACK_SIZE / sizeof(ULONG)];
static CHAR names[THREAD_COUNT][24];

TX_MUTEX ticket_mutex;


void tx_application_define(void *first_unused_memory)
{
    tx_mutex_create(&ticket_mutex, "ticket_mutex", TX_NO_INHERIT);

    for (UINT i = 0; i < THREAD_COUNT; i++) {

        snprintf(names[i], sizeof names[i], "Agent %u", i + 1U);

        tx_thread_create(
            &thread_pfr[i],
            names[i],
            thread_ticket_agent_entry,
            0,
            thread_stack[i],
            sizeof thread_stack[i],
            10,            // Priority level (Lower number = higher priority)
            10,            // Preemption threshold
            TX_NO_TIME_SLICE,
            TX_AUTO_START  // Start running immediately
        );

    }
}