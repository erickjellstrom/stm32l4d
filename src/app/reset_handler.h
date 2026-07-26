#ifndef RESET_HANDLER_H
#define RESET_HANDLER_H

#include <stdint.h>

struct SystemStatus {
    uint32_t magic_number;
    uint32_t last_error_code;
    uint8_t reset_cnt;
};

extern __attribute__((section(".noinit"))) struct SystemStatus sys_status;


void reset_handler(void);
void reset_handler_init(void);

#endif //RESET_HANDLER_H