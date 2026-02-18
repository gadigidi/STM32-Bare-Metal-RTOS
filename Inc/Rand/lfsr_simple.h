#ifndef LFSR_SIMPLE_H
#define LFSR_SIMPLE_H

#include <stdint.h>

void lfsr_init(uint16_t seed);
uint16_t lfsr_next(void);

#endif

