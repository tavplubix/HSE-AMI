#include <inttypes.h>
#include <stdio.h>


int
main()
{
    int n, w;
    scanf("%d%d", &n, &w);
    int32_t max = 1;
    max <<= n;
    int32_t halfmax = max >> 1;
    for (int32_t i = 0; i < max; ++i) {
        int32_t sign_magnitude = i;
        if (halfmax <= i) {
            sign_magnitude = -(i - halfmax);
        }
        printf("|%*"PRIx32"|%*"PRId32"|%*"PRId32"|\n",
                w, (uint32_t) i, w, i, w, sign_magnitude);
    }
}

