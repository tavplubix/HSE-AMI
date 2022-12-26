#include <stdio.h>

#include "4.c"


int main() {
    uint32_t a, b;
    int n;
    while (scanf("%x%x%d", &a, &b, &n) != EOF) {
        printf("%x\n", fixed_mul(a, b, n));
    }
}


