

#include "tx_api.h"
#include "main.h"
#include "random.h"
#include "adc.h"
#include "data.h"
#include "tx_app_pls.h"
#include "timer.h"

static uint8_t knives[4] = {1, 1, 1, 1};

static void get_knives(uint32_t n, char *name)
{
    uint8_t num_knives = 0;
    

    while (num_knives != 2) {

        tx_mutex_get(&knives_mutex, TX_WAIT_FOREVER);
        if (knives[n] == 1) {
            knives[n] = 0;
            num_knives++;
            printf("%s got knive %u\r\n", name);        
        }

        if (knives[(n+1)%3] == 1) {
            knives[(n+1)%3] = 0;
            num_knives++;
            printf("%s got knive %u\r\n", name, (n+1)%3);        
        }
        tx_mutex_put(&knives_mutex);

    }
}

// This is the actual code our thread will run
void thread_pls_entry(uint32_t input)
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

        tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);
        printf("%s got semaphore\r\n", name);        

//        get_knives(input, name);
        
        // work time
        work_time = random_integer(1, 9) * 100;
        //tim2_delay_ms(work_time);
        tx_thread_sleep(work_time); // @10ms tick rate: delay = rand * 100ms 
        printf("%s has worked  %u seconds\r\n", name, work_time);
        
        /* Release the semaphore.  */
        tx_semaphore_put(&semaphore_0);
        printf("%s released semaphore\r\n", name);
        
        // rest time
        rest_time = random_integer(1, 4) * 100;
        tx_thread_sleep(rest_time); // @10ms tick rate: delay = rand * 100ms  
        printf("%s rested %u seconds\r\n", name, rest_time / 100);
    }

}