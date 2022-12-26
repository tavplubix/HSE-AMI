#include <stdint.h>

int32_t satsum(int32_t v1, int32_t v2) {
    int32_t sum;
    if (__builtin_add_overflow(v1, v2, &sum)) {
        if (v1 < 0 && v2 < 0) {
            return (uint32_t) 1 << 31;
        }
        return ~((uint32_t) 1 << 31);   // if (0 < v1 && 0 < v2)
    } else {
        return sum;
    }
}

