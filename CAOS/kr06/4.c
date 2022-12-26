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
    uint32_t wsum = 0, xsum = 0;
    uint32_t from, to;
    enum { BUF_SIZE = 16, WPOS = 1, XPOS = 2 };
    char buf[BUF_SIZE];
    while (scanf("%"SCNx32"-%"SCNx32"%s", &from, &to, buf) != EOF) {
        if (buf[WPOS] == 'w') {
            wsum += to - from;
        }
        if (buf[XPOS] == 'x') {
            xsum += to - from;
        }
        skip_str();
    }
    printf("%"SCNu32"\n""%"SCNu32"\n", wsum, xsum);
}


