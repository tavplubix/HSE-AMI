#include "5.c"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    char *str = NULL;
    size_t n = 0;
    getline(&str, &n, stdin);
    printf("===============================================================\n");
    int ret = mysystem(str);
    printf("%d ===============================================================\n", ret);
    ret = system(str);
    printf("%d ===============================================================\n", ret);
}
