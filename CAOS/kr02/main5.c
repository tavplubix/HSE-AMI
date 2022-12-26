#include <stdio.h>

#include "5.c"

int main() {
    while(1) {
        int val;
        scanf("%d", &val);
        unsigned short hf;
        to_half_float(val, &hf);
        printf("%hx\n", hf);
    }
}


