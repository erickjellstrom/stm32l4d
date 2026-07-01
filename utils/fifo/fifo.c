#include "fifo.h"

enum ret_status fifo_init(struct fifo *f, uint8_t *data, uint16_t len)
{
    // Safety check for null pointers or zero length
    if (f == NULL || data == NULL || len == 0) {
        return STATUS_FAILED;
    }

    f->len = len; 
    f->num_items = 0;
    f->head = 0;
    f->tail = 0;
    f->data = data;

    return STATUS_SUCCESS;
}

enum ret_status fifo_put(struct fifo *f, uint8_t input)
{
    // Use num_items directly to check for a full buffer
    if (f->num_items >= f->len) {
        return STATUS_FIFO_FULL;
    }

    f->data[f->tail] = input;
    f->tail = (f->tail + 1) % f->len; // Fixed undefined behavior
    f->num_items++;

    return STATUS_SUCCESS;
}

enum ret_status fifo_get(struct fifo *f, uint8_t *ret_item)
{
    // Safety check for null pointer and empty status
    if (f == NULL || ret_item == NULL || f->num_items == 0) {
        return STATUS_FIFO_EMPTY;
    }

    *ret_item = f->data[f->head];
    f->head = (f->head + 1) % f->len; // Cleanly wrap index instantly
    f->num_items--;

    return STATUS_SUCCESS;
}

void fifo_print(const struct fifo *f)
{
    if (f == NULL) {
        printf("FIFO Error: Null Pointer\n");
        return;
    }
    if (f->num_items == 0) {
        printf("FIFO is empty. [ ]\n\n");
        return;
    }

    printf("FIFO Context: (Count: %d/%d, Head: %d, Tail: %d)\n", 
           f->num_items, f->len, f->head, f->tail);
    printf("Data: [ ");

    uint8_t current_index = f->head;
    for (uint8_t i = 0; i < f->num_items; i++) {
        printf("%d ", f->data[current_index]);
        current_index = (current_index + 1) % f->len;
    }
    printf("]\n\n");
}