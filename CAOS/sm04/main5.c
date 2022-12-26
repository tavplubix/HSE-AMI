#include <stdio.h>
#include <string.h>

#include "5.c"


int main(int argc, char** argv) {
    FILE *f = fopen(argv[1], "r");
    char *line = getline2(f);
    for (size_t i = 0; i < 16; ++i) {
    //    printf("\n i = %ld, s[i] = %d \n", i, (int) line[i]);
    }
    //printf("%ld\n", strlen(line));
    if (line == NULL)
        printf("NULL");
    else
        printf("%s", line);
    free(line);
}


