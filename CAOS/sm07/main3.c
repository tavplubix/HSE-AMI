#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "3.c"


int cmp(const void *p1, const void *p2, void *user) {
    int i1 = *(int *) p1;
    int i2 = *(int *) p2;
    if (i1 < i2)
        return -1;
    else if (i1 > i2)
        return 1;
    else
        return 0;
}

int main() {
    enum { SIZE = 128 };
    int data[SIZE];
    size_t count = 0;
    int i;
    while (scanf("%d", &i) != EOF) {
        data[count++] = i;
    }
    qsort_r(data, count, sizeof(int), cmp, NULL);
    for (size_t i = 0; i < count; ++i)
        printf("%d ", data[i]);
    putchar('\n');

    scanf("%d", &i);
    struct BSearchResult bsr = bsearch2(&i, data, count, sizeof(int), cmp, NULL);
    printf("low = %zu, high = %zu, res = %d\n", bsr.low, bsr.high, bsr.result);
}




