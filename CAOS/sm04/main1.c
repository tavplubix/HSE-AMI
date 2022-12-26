#include <stdio.h>
#include <string.h>

#include "1.c"

int main() {
    char str1[1024], str2[1024];
    scanf("%s%s", str1, str2);
    printf("\n%d\n%d\n", mystrcmp(str1, str2), strcmp(str1, str2));
}


