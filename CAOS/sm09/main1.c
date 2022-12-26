#include <stdio.h>

int A, B;

extern void print_fib();

int main() {
    scanf("%d%d", &A, &B);
    print_fib();
}


