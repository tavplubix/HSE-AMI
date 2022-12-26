#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { BITS_IN_BYTE = 8 };


bool fits_into_intx_t(int64_t val, unsigned x) {
    if (x == 0) {
        return false;
    }
    
    // val is in [-2^63, 2^63-1]
    uint64_t uval = val;
    uint64_t mask = ~(uint64_t)0 << (x - 1);
    
    // if val >= 0 then uval == val
    //     if val fits into intx_t 
    //         then val is in [0, 2^(x-1) - 1]
    //         then uval is in [0, 2^(x-1) - 1]
    //         then (uval & mask) == 0
    // if val < 0 then uval == val + 2^64 and uval are in [2^63, 2^64 - 1]
    //     if val fits into intx_t
    //         then val is in [-2^(x-1), -1]
    //         then uval is in [2^64 - 1 - 2^(x-1) + 1, 2^64 - 1]
    //         then (uval & mask) == mask
    return (uval & mask) == 0 || (uval & mask) == mask;
}

int
is_convertable_to_intx_t(const char *str)
{
    enum { BASE = 10 };
    char *first_invalid;
    errno = 0;
    int64_t res = strtol(str, &first_invalid, BASE);
    if (errno == ERANGE || str == first_invalid || *first_invalid) {
        return -1;
    }
    if (fits_into_intx_t(res, BITS_IN_BYTE * sizeof(int8_t))) {
        return sizeof(int8_t);
    } else if (fits_into_intx_t(res, BITS_IN_BYTE * sizeof(int16_t))) {
        return sizeof(int16_t);
    } else if (fits_into_intx_t(res, BITS_IN_BYTE * sizeof(int32_t))) {
        return sizeof(int32_t);
    }
    return -1;
}

int
main(int argc, char **argv)
{
    for (int i = 1; i != argc; ++i) { 
        printf("%d\n", is_convertable_to_intx_t(argv[i]));
    }
    if (argc == 1) {
        putchar('\n');
    }
}

