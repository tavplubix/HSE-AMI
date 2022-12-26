#include <stdio.h>

extern int P, Q, R, Z;

extern int process();

int main() {
    scanf("%d%d%d%d", &P, &Q, &R, &Z);
    printf("%d\n", process());
}


