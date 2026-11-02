
#include "tx_api.h"
#include "main.h"
#include "random.h"
#include "adc.h"
#include "data.h"
#include "tx_app_ticket_agent.h"

// This is the actual code our thread will run
void thread_msg_entry(ULONG input)
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

static uint8_t tickets = 255;

void thread_ticket_agent_entry(ULONG input)
{
    TX_THREAD *self = tx_thread_identify();
    const CHAR *name = (self != TX_NULL) ? self->tx_thread_name : "unknown";

    randomize(adc_read_noise() % 10);

    uint8_t ticket_in_work;
    uint8_t work_time;
    uint8_t rest_time;

    while(1)
    {
        // Acquire the mutex (wait forever if unavailable)
        if (tx_mutex_get(&ticket_mutex, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            // CRITICAL SECTION: Safe to read/write shared variable
            if (tickets > 0)
            {
                ticket_in_work = tickets;
                tickets--; // Sell a ticket
                printf("%s is selling ticket %u \r\n", name, ticket_in_work);
            }
            
            // Release the mutex
            tx_mutex_put(&ticket_mutex);
        }

        // work time
        work_time = random_integer(1, 9) * 100;
        tx_thread_sleep(work_time); // @10ms tick rate: delay = rand * 100ms 
        printf("%s sold ticket %u in %u seconds\r\n", name, ticket_in_work, work_time);
        
        // rest time
        rest_time = random_integer(1, 4) * 100;
        tx_thread_sleep(rest_time); // @10ms tick rate: delay = rand * 100ms  
        printf("%s rested %u seconds\r\n", name, rest_time / 100);
    }
}
