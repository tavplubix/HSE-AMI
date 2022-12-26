#define _GNU_SOURCE

#include <stdio.h>

#include "2.c"

int main() {
    enum { SIZE = 128 };
    int data[SIZE];
    int order[SIZE];
    size_t count = 0;
    int i;
    while (scanf("%d", &i) != EOF) {
        data[count++] = i;
    }
    process(count, data, order);
    for (size_t i = 0; i < count; ++i)
        printf("%d ", order[i]);
    putchar('\n');

}


