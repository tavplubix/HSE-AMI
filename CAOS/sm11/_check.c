#include <stdio.h>


int main() {
#pragma omp parallel
    for (unsigned i = 0xFFFFFFFFu; i != 0; --i) {
        if (i % 1000000u)
            printf("%u\n", i);
        unsigned a = (i * 0xCCCCCCCDull) >> 35;
        unsigned b = i / 10u;
        if (a != b) {
            printf("%u\t%u\t%u\t%n\n", i, a, b, a-b);
        }
    }
}



