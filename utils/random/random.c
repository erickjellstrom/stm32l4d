#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <stdint.h>

#include "random.h"


void randomize(uint32_t seed)
{
//  srand((uint32_t)time(NULL));
	srand(seed);
}

uint16_t random_integer(uint8_t rlow, uint8_t rhigh)
{
  uint8_t scale = rhigh - rlow + 1;  
  uint32_t r = rand();
  
  float r_real = (float)r / ((float) RAND_MAX + 1);
  r_real = r_real * scale;

  uint16_t r_out = (uint16_t) r_real;
  return r_out + rlow;
}

float random_real()
{

  uint32_t r = rand();  
  float r_real = (float)r / ((float) RAND_MAX + 1);
  return r_real;
}

uint8_t random_chance()
{
	float r = random_real();
  if (r > 0.5) return 1;
  else return 0;  
}
