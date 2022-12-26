#include <stdio.h>

extern int A, B, C, D, R;

extern void
process();

int
main()
{
    scanf("%d%d%d%d", &A, &B, &C, &D);
    process();
    printf("%d\n", R);
}


