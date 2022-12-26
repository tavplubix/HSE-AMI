#include <stdio.h>


int main(int argc, char **argv) {
    FILE *fd = fopen(argv[1], "w");
    double d;
    int b;
    while (scanf("%lf", &d) != EOF) {
        b = fwrite(&d, sizeof(double), 1, fd);
    }
    fclose(fd);
}


