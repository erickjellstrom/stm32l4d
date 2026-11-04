

#include "tx_api.h"
#include "main.h"
#include "random.h"
#include "adc.h"
#include "data.h"
#include "tx_app_ticket_agent.h"
#include "timer.h"

// This is the actual code our thread will run
void thread_pls_entry(ULONG input)
{

    TX_THREAD *self = tx_thread_identify();
    const CHAR *name = self->tx_thread_name; 
    //const CHAR *name = (self != TX_NULL) ? self->tx_thread_name : "unknown";

    uint32_t loop_count = 0;   

    randomize(adc_read_noise() % 10);

    uint16_t work_time;
    uint16_t rest_time;

    while(1)
    {
        // work time
        work_time = random_integer(1, 9) * 100;
        tim2_delay_ms(work_time);
        //tx_thread_sleep(work_time); // @10ms tick rate: delay = rand * 100ms 
        printf("%s is working  %u seconds\r\n", name, work_time);
        
        // rest time
        rest_time = random_integer(1, 4) * 100;
        tx_thread_sleep(rest_time); // @10ms tick rate: delay = rand * 100ms  
        printf("%s rested %u seconds\r\n", name, rest_time / 100);
    }

}