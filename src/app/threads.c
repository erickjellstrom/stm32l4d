
#include "tx_api.h"
#include "main.h"

// This is the actual code our thread will run
void thread_msg_entry(void)
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