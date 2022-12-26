#include <stdio.h>

unsigned long long A, B;

extern void mul64p(unsigned long long A, unsigned long long B);


int main() {
    scanf("%llx%llx", &A, &B);
    mul64p(A, B);
}




