#include <stdio.h>

#include "4.c"


int main() {
    float f1, f2;
    scanf("%f%f", &f1, &f2);
    printf("%d\n", cmpf(&f1, &f2));
}



