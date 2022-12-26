#include <stdio.h>



extern int addbe(size_t n, const unsigned char *pa, const unsigned char *pb, unsigned char *pc);



int main() {
    unsigned char pa[128], pb[128], pc[128];
    int n;
    scanf("%d", &n);
    unsigned tmp;
    for (int i = 0; i < n; ++i) {
        scanf("%x", &tmp);
        pa[i] = tmp;
    }
    for (int i = 0; i < n; ++i) {
        scanf("%x", &tmp);
        pb[i] = tmp;
    }

    int res =  addbe(n, pa, pb, pc);

    for (int i = 0; i < n; ++i) {
        tmp = pc[i];
        printf("%x ", tmp);
    }
    printf("\n%d\n", res);

}







