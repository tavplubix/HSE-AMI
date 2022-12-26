#include <stdio.h>

extern unsigned int satsum(unsigned int a, unsigned int b);


int main() {
    unsigned int a, b;
    scanf("%u%u", &a, &b);
    printf("%u\n", satsum(a, b));
}

