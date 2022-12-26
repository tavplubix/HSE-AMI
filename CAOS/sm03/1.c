#include <stdint.h>

uint32_t satsum(uint32_t v1, uint32_t v2) {
    uint32_t sum = v1 + v2;
    if (sum < v1 || sum < v2) {
        return ~((uint32_t)0);
    } else {
        return sum;
    }
}

