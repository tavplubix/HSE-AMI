#include <stdio.h>

unsigned long long A, B;


typedef unsigned long long ull;

int main() {
    scanf("%llx%llx", &A, &B);
    unsigned __int128 a = A, b = B, r;
    r = a * b;
    printf("%llx %llx\n", (ull)(r >> 64), (ull)((r << 64) >> 64));
}
