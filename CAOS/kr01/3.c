#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

uint32_t
diff(int32_t x, int32_t y)
{
    uint32_t ux = x;  // ux = x or ux = x + 2^32;
    uint32_t uy = y;  // uy = y or uy = y + 2^32
    return ux - uy;   // 0 <= ux - uy = x - y + n*2^32 = x - y (mod 2^32)
}

int
main() {
    int32_t pervious, current;
    scanf("%"SCNd32, &pervious);
    if (scanf("%"SCNd32, &current) == EOF) {
        fputs("0\n", stdout);
        return 0;
    }
    int32_t first_in_seq = current;
    uint32_t max_diff = 0;
    if (pervious < current) {
        first_in_seq = pervious;
        max_diff = diff(current, pervious);
    }
    pervious = current;
    while (scanf("%"SCNd32, &current) != EOF) {
        if (pervious <= current && max_diff < diff(current, first_in_seq)) {
            max_diff = diff(current, first_in_seq);
        } else if (pervious > current) {
            first_in_seq = current;
        }
        pervious = current;
    }
    printf("%"PRIu32"\n", max_diff);
}

