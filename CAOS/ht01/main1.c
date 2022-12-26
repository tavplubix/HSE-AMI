#include <stdio.h>

extern double A, B, C;

extern void process();

int main() {
    A = 42;
    B = 8;
    process();
    printf("%f\n", C);
}


