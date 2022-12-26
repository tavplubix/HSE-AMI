#include <stdio.h>
#include <inttypes.h>

int main() {
    float f;
    uint32_t d;
    while (scanf("%"SCNu32, &d) != EOF) {
        f = d;
        d = f;
        printf("%"PRIu32", \t%f\n", d, f);
    }
}

