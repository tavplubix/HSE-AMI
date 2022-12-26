#include <stdio.h>


struct Data
{
    int U, V, W, X;
};

extern long long process(const struct Data *p);

int main() {
    struct Data d;
    scanf("%d%d%d%d", &(d.U), &(d.V), &(d.W), &(d.X));
    printf("%lld\n", process(&d));
}






