#include <stdio.h>

typedef int STYPE;
typedef unsigned int UTYPE;
#include "3.c"

int
main(void) {
    STYPE a;
    scanf("%d", &a);
    printf("%d\n", bitcount(a));
}
