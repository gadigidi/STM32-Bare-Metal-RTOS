#include "rand_weighted.h"
#include "lfsr_simple.h"

uint8_t rand_weighted(const uint8_t *weights, uint8_t count)
{
    uint16_t sum = 0;

    // 1) Sum Weights
    for (uint8_t i = 0; i < count; i++)
        sum += weights[i];

    // 2) rand num in the range 0..sum-1
    uint16_t r = lfsr_next() % sum;

    // 3) find weight 
    uint16_t acc = 0;

    for (uint8_t i = 0; i < count; i++)
    {
        acc += weights[i];

        if (r < acc)
            return i;
    }

    return 0; // fallback (shouldn't happend)
}

