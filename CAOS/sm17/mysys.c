#include "2.c"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    printf("===============================================================\n");
    mysys(argv[1]);
    printf("===============================================================\n");
    system(argv[1]);
    printf("===============================================================\n");
}