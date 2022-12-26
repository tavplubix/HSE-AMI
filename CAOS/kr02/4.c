#include <stdint.h>


enum
{
    BITS_FOR_SGN = 1,
    BITS_FOR_EXP = 8,
    BITS_FOR_MNT = 23
};


int
get_parts_of_float(const void *f, uint32_t *exp, uint32_t *mnt)
{
    uint32_t number = *(const uint32_t *) f;
    int sign = number >> (BITS_FOR_EXP + BITS_FOR_MNT);
    uint32_t mask = ~(uint32_t) 0 >> BITS_FOR_SGN;
    *exp = (number & mask) >> BITS_FOR_MNT;
    mask >>= BITS_FOR_EXP;
    *mnt = number & mask;
    return sign;
}


int
cmpf(const void *p1, const void *p2)
{
    uint32_t exp1, mnt1;
    int sign1 = get_parts_of_float(p1, &exp1, &mnt1);
    uint32_t exp2, mnt2;
    int sign2 = get_parts_of_float(p2, &exp2, &mnt2);

    if (sign1 != sign2) {
        return sign2 - sign1;
    }

    if (sign1) {
        sign1 = -1;
    } else {
        sign1 = 1;
    }

    if (exp1 > exp2) {
        return 1 * sign1;
    } else if (exp1 < exp2) {
        return -1 * sign1;
    }

    if (mnt1 > mnt2) {
        return 1 * sign1;
    } else if (mnt1 < mnt2) {
        return -1 * sign1;
    }  
    return 0;
}

