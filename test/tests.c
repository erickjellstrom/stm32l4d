#include "tests.h"


void random_test(void)
{
    printf("We are testing random\n");
    randomize(100);
    uint16_t rnum = random_integer(3, 9);
    float rflnum = random_real();
}