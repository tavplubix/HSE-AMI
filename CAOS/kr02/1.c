#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

bool read_set(uint32_t *res) {
    int elem;
    *res = 0;
    while (scanf("%d", &elem) != EOF) {
        if (elem < 0) {
            return true;
        }
        *res |= (uint32_t) 1 << elem;
    }
    return false;
}


int
main()
{
    uint32_t res_set = 0;
    uint32_t cur_set;
    while (read_set(&cur_set)) {
        res_set ^= cur_set;
    }
    printf("%"PRIx32"\n", res_set);
}


