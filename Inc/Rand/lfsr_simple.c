#include <stdint.h>

static uint16_t lfsr = 0xACE1u; //Default if no seed provided

void lfsr_init(uint16_t seed)
{
    if (seed == 0u) seed = 1u;
    lfsr = seed;
}

uint16_t lfsr_next(void)
{
    // taps: 16,14,13,11  (maximal length)
    uint16_t bit =
        ((lfsr >> 0) ^
         (lfsr >> 2) ^
         (lfsr >> 3) ^
         (lfsr >> 5)) & 1u;

    lfsr = (uint16_t)((lfsr >> 1) | (bit << 15));
    return lfsr;
}
