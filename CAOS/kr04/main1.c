#include <stdio.h>


int process(int v3, int v2, long long v1) {
    return v1*v2 + v3;
}

extern int forwardargs(long long v1, int v2, int v3);

int main() {
    long long v1;
    int v2, v3;
    scanf("%lld%d%d", &v1, &v2, &v3);
    printf("%d\n", forwardargs(v1, v2, v3));
}



