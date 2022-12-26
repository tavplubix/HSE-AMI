#include <stdio.h>


extern int process(int a, int b, int *res);

int main() {
    int a, b, r;
    scanf("%d%d", &a, &b);
    printf("%d ", process(a, b, &r));
    printf("%d\n", r);
}




