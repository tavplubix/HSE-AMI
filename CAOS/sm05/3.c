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

static void
parts_of_float(float f, int *sign, uint32_t *exp, uint32_t *mnt)
{
    union FloatUint32 number;
    number.f = f;
    *sign = number.d >> (BITS_FOR_EXP + BITS_FOR_MNT);
    uint32_t mask = ~(uint32_t) 0 >> BITS_FOR_SGN;
    *exp = (number.d & mask) >> BITS_FOR_MNT;
    mask >>= BITS_FOR_EXP;
    *mnt = number.d & mask;
}


FPClass
fpclassf(float value, int *psign)
{
    uint32_t exp, mnt;
    parts_of_float(value, psign, &exp, &mnt);
    uint32_t max_exp = ~(~(uint32_t) 0 << BITS_FOR_EXP);
    if (exp == max_exp && mnt) {
        *psign = 0;
        return FFP_NAN;
    }
    if (exp == 0 && mnt == 0) {
        return FFP_ZERO;
    }
    if (exp == max_exp && mnt == 0) {
        return FFP_INF;
    }
    if (exp == 0 && mnt) {
        return FFP_DENORMALIZED;
    }
    return FFP_NORMALIZED;
}


