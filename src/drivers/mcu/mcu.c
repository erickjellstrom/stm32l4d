#include "mcu.h"

void mcu_delay(volatile int count) {
    while (count--) {
        // Waste CPU cycles to create a delay
    }
}
