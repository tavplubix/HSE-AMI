#include <stdio.h>

#include "3.c"


int main() {
    float f;
    while (scanf("%f", &f) != EOF) {
        int psign = 2;
        FPClass fpc = fpclassf(f, &psign);
        printf("psign = %d, fpc = %d\n", psign, (int) fpc);
    }
}


