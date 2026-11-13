
#include "tx_api.h"
#include "tx_thread.h"
#include "main.h"
#include "tx_thread_pls.h"

// Define properties for our test thread
#define NUM_PLFRS 5
#define THREAD_STACK_SIZE 1024

TX_THREAD thread_plfrs[NUM_PLFRS];
static ULONG thread_stack[NUM_PLFRS][THREAD_STACK_SIZE / sizeof(ULONG)];
static CHAR names[NUM_PLFRS][24];

TX_MUTEX knives_mutex;

TX_SEMAPHORE semaphore_0;



void _tx_execution_thread_enter(void)
{
    if (_tx_thread_current_ptr == &thread_plfrs[0])
    {
        ITM->PORT[0].u8 = 0xaa; // Channel 0 HIGH
    }
}

void _tx_execution_thread_exit(void)
{
    if (_tx_thread_current_ptr == &thread_plfrs[0])
    {
        ITM->PORT[0].u8 = 0xbb; // Channel 0 LOW
    }
}

void _tx_execution_isr_enter(void)
{
}

void _tx_execution_isr_exit(void)
{
}

void _tx_execution_initialize(void)
{
    /* Called once when ThreadX kernel initializes */
}

void tx_application_define(void *first_unused_memory)
{
    ITM->PORT[0].u8 = 0xaa;

    tx_mutex_create(&knives_mutex, "ticket_mutex", TX_NO_INHERIT);

    tx_semaphore_create(&semaphore_0, "semaphore_0", 3);

    for (uint8_t i = 0; i < NUM_PLFRS; i++) {

        snprintf(names[i], sizeof names[i], "Philosofer %u", i + 1U);

        tx_thread_create(
            &thread_plfrs[i],
            names[i],
            thread_pls_entry,
            i,
            thread_stack[i],
            sizeof thread_stack[i],
            10,            // Priority level (Lower number = higher priority)
            10,            // Preemption threshold
            1,             // Time slice in ticks (10ms)
            TX_AUTO_START  // Start running immediately
        );

    }
}