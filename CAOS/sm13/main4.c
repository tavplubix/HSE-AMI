#include "4.c"
#include <stdio.h>

int
main()
{
    int perms;
    size_t size;
    char buf[32];
    while (scanf("%o%zu", &perms, &size) != EOF) {
        perms_to_str(buf, size, perms);
        printf("%s\n", buf);
    }
}

