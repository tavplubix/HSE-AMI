#include <stdio.h>

int
main(void)
{
    int a, b, n;
    scanf("%d%d%d", &a, &b, &n);
    printf("%*s", n, " ");
    for (int i = a; i < b; ++i) {
        printf(" %*d", n, i);
    }
    printf("\n");
    for (int i = a; i < b; ++i) {
        printf("%*d", n, i);
        for (int j = a; j < b; ++j) {
            printf(" %*lld", n, (long long) i * j);
        }
        printf("\n");
    }
}

