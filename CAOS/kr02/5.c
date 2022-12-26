
void
to_half_float(int value, unsigned short *hf)
{
    enum
    {
        SGN = 1,
        EXP = 5,
        MNT = 10,
        BIAS = 15,
        INF_MNT = 31,
        MAX_VAL = 65519     // 1111'1111'1110'1111 ~ 1.111'1111'111 * 2^15
    };
    
    if (value == 0) {
        *hf = 0;
        return;
    }

    unsigned short sgn = 0;
    if (value < 0) {
        sgn = 1;
    }
    
    if (value < -MAX_VAL || MAX_VAL < value) {
        *hf = sgn << (EXP + MNT) | INF_MNT << MNT;
        return;
    }
    if (value < 0) {
        value = -value;
    }
    unsigned short uval = value;

    unsigned short mask = ~(unsigned short) 0;
    mask <<= MNT + 1;
    unsigned short exp = BIAS + MNT;
    while (!(uval & mask)) {
        uval <<= 1;
        --exp;
    }
    unsigned short last_bit = 0;
    while (uval & mask) {       
        last_bit = uval & 1;
        uval >>= 1;
        ++exp;
    }
    uval += last_bit;
    if (uval & mask) {
        uval >>= 1;
        ++exp;
    }
    
    unsigned short mnt = uval & (~mask >> 1);
    *hf = sgn << (EXP + MNT) | exp << MNT | mnt;
}





