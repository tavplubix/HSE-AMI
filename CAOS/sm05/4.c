#include <stdint.h>


uint32_t
fixed_mul(uint32_t a, uint32_t b, int n)
{
    if (n < 0 || 32 < n) {
        return 0;
    }
    uint64_t prod = a;
    prod *= b;
    uint64_t extra_fract_part_mask = ~(~(uint64_t) 0 << n);
    uint64_t extra_fract_part = prod & extra_fract_part_mask;
    uint64_t first_extra_digit = extra_fract_part 
                                    - (prod & (extra_fract_part_mask >> 1));
    extra_fract_part -= first_extra_digit;
    prod >>= n;
    if (first_extra_digit) {
        if (extra_fract_part) {     // round up
            ++prod;
        } else {        // round to nearest even
            prod += prod & 1u;
        }
    }
     
    enum { MAX_NUMBER_OF_BITS = 32 };
    uint64_t max = ~(~(uint64_t) 0 << MAX_NUMBER_OF_BITS);
    if (prod > max) {
        prod = max;
    }
    return prod;
}


