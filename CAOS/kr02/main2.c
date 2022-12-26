#include <stdio.h>
#include <string.h>

#include "2.c"

int main() {
    const char * str1 = "     ", *str2 = " ";
    //scanf("%s%s", str1, str2);
    printf("\n%d\n", mystrspccmp(str1, str2));
}


