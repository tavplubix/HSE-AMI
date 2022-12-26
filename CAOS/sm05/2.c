#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

enum { MAX_BITS_REPR = 24 };

int
is_convertable_to_float(uint32_t number)
{
    if (number == 0) {
        return 1;
    }
    while ((number & 1u) == 0) {
        number >>= 1;
    }
    uint32_t mask = 0;
    mask = ~mask << (MAX_BITS_REPR);
    if (number & mask) {
        return 0;
    } else {
        return 1;
    }
}

int
main()
{
    uint32_t number;
    while (scanf("%"SCNu32, &number) != EOF) {
        printf("%d\n", is_convertable_to_float(number));
    }
}

