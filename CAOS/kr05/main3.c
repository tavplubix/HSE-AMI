#include "3.c"

#include <stdio.h>

int main()
{
    char buf[128];
    while (scanf("%s", buf) != EOF) {
        printf("%o\n", parse_rwx_permissions(buf));
    }
}

