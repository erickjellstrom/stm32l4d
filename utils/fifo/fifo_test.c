#include "main.h"
#include "fifo.h"

void fifo_test(void)
{
    printf("fifo_test started/n");

    struct fifo fifo; 
    uint8_t fdata[8];
    fifo_init(&fifo, fdata, 8);

    for (int i=0; i<4; i++) {
        fifo_put(&fifo, 64+i);
    }

    fifo_print(&fifo);
}
