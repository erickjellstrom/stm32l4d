#include "tx_api.h"
#include "main.h"

// Define properties for our test thread
#define THREAD_STACK_SIZE 1024
TX_THREAD my_thread;
uint8_t thread_stack[THREAD_STACK_SIZE];


// This is the actual code our thread will run
void my_thread_entry(ULONG thread_input)
{

    uint32_t loop_count = 0;   
    while(1)
    {
        // Toggle Pin 14
        GPIOB->ODR ^= GPIO_ODR_OD14;
        
        printf("Hello Bare Metal! Loop count: %lu\r\n", loop_count++);
        // I have a 10ms tick rate
        tx_thread_sleep(50); // @10ms tick rate: delay = 500ms 
    }
}

// ThreadX automatically calls this function right before launching the kernel
void tx_application_define(void *first_unused_memory)
{
    // Create the test thread
    tx_thread_create(
        &my_thread,
        "Blinky Thread",
        my_thread_entry,
        0,
        thread_stack,
        THREAD_STACK_SIZE,
        10,            // Priority level (Lower number = higher priority)
        10,            // Preemption threshold
        TX_NO_TIME_SLICE,
        TX_AUTO_START  // Start running immediately
    );
}
