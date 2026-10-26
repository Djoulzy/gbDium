#ifndef TEST_RAND_H
#define TEST_RAND_H
#include <stdint.h>
void initrand(uint16_t seed);
uint8_t test_rand(void);
#define rand test_rand
#endif
