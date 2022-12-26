#include <stdio.h>

extern void process(unsigned *data, int count);

int main() {
    unsigned arr[64];
    int c = 0;
    scanf("%d", &c);
    for (int i = 0; i < c; ++i)
        scanf("%d", arr + i);
    process(arr, c);
    for (int i = 0; i < c; ++i)
        printf("%d ", arr[i]);
    printf("\n");

}




