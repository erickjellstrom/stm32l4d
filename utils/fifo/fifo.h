#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

struct fifo {
    uint16_t len;
    uint16_t num_items;
    uint16_t head;
    uint16_t tail;
    uint8_t *data;
};

enum ret_status { 
    STATUS_SUCCESS,
    STATUS_FAILED,
    STATUS_FIFO_FULL,
    STATUS_FIFO_EMPTY // Added for clean data fetching
};

enum ret_status fifo_init(struct fifo *f, uint8_t *data, uint16_t len);
enum ret_status fifo_put(struct fifo *f, uint8_t input);
enum ret_status fifo_get(struct fifo *f, uint8_t *ret_item);
void fifo_print(const struct fifo *f);

#endif // FIFO_H