#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdlib.h>

void randomize(uint32_t seed);
uint16_t random_integer(uint8_t rlow, uint8_t rhigh);
float random_real();
uint8_t random_chance();

#endif //RANDOM_H
