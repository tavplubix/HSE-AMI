#include <inttypes.h>
#include <stdio.h>

#include "4.c"

int
main() 
{
    int64_t a, b, c;
    scanf("%ld%ld%ld", &a, &b, &c);
    printf("%ld\n", addmod(a, b, c));
}
