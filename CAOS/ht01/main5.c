#include <stdio.h>

extern int myrand();

int main() {
    srand(12);
    for (int i = 0; i < 10; i++)
    printf("%d\n", myrand());
}



