#include <stdio.h>
#include "1.c"

int
main(void) {
    uint32_t a, b;
    scanf("%u%u", &a, &b);
    printf("%u + %u = %u\n", a, b, satsum(a, b));
}

