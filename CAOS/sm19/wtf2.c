#include <stdio.h>
#include <memory.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>
int main() {
    int x;
    while (1) {
        int result = scanf("%d", &x);
        if (result == EOF) {
            return 0;
        }
        printf("%d\n", -x);
        fflush(stdout);
   }
}
