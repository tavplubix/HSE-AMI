#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

enum
{
    BITS_FOR_SGN = 1,
    BITS_FOR_EXP = 8,
    BITS_FOR_MNT = 23
};

union FloatUint32
{
    float f;
    uint32_t d;
};

int
main() {
    union FloatUint32 number;
    while (scanf("%f", &number.f) == 1) {
        uint32_t sign = number.d >> (BITS_FOR_EXP + BITS_FOR_MNT);
        uint32_t mask = ~(uint32_t) 0 >> BITS_FOR_SGN;
        uint32_t exp = (number.d & mask) >> BITS_FOR_MNT;
        mask >>= BITS_FOR_EXP;
        uint32_t mnt = number.d & mask;
        printf("%"PRIu32" %"PRIu32" %"PRIx32"\n", sign, exp, mnt);
    }
}

