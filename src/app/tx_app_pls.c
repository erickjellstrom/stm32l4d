
#include "tx_api.h"
#include "main.h"
#include "tx_thread_pls.h"

// Define properties for our test thread
#define NUM_PLFRS 5
#define THREAD_STACK_SIZE 1024

TX_THREAD thread_plfrs[NUM_PLFRS];
static ULONG thread_stack[NUM_PLFRS][THREAD_STACK_SIZE / sizeof(ULONG)];
static CHAR names[NUM_PLFRS][24];

//TX_MUTEX ticket_mutex;


void tx_application_define(void *first_unused_memory)
{
 //   tx_mutex_create(&ticket_mutex, "ticket_mutex", TX_NO_INHERIT);

    for (UINT i = 0; i < NUM_PLFRS; i++) {

        snprintf(names[i], sizeof names[i], "Philosofer %u", i + 1U);

        tx_thread_create(
            &thread_plfrs[i],
            names[i],
            thread_pls_entry,
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