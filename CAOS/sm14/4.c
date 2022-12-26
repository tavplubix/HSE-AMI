#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

static void
skip_str()
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int
main()
{
    uint32_t sum = 0;
    uint32_t from, to;
    while (scanf("%"SCNx32"-%"SCNx32, &from, &to) != EOF) {
        sum += to - from;
        skip_str();
    }
    printf("%"SCNu32"\n", sum);
}


