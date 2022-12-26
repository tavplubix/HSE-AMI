#include <stdio.h>

extern void process();
unsigned long long A, B;

int main() {
    scanf("%llu%llu", &A, &B);
    process();
    unsigned long long a = A, b = B;
    printf("%llu\n", a * 2 - b/2);
}






