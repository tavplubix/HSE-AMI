#include <stdio.h>
#include <string.h>

#include "2.c"

int main() {
    char str1[1024];
    str1[0] = 0;
    //scanf("%s", str1);
    normalize_path(str1);
    printf("\n%s\n", str1);
}


