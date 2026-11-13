#ifndef TX_APP_PLS_H
#define TX_APP_PLS_H

extern TX_SEMAPHORE semaphore_0;
extern TX_MUTEX knives_mutex;

void tx_application_define(void *first_unused_memory);

#endif //TX_APP_PLS_H