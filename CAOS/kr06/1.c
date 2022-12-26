#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

typedef long double (*func) (long double);

int
main(int argc, char **argv)
{
    long double x0 = strtold(argv[1], NULL);
    for (int i = argc - 1; i > 2; i -= 2) {
        void *dl = dlopen(argv[i - 1], RTLD_LAZY);
        if (dl == NULL) {
            continue;
        }
        func f = dlsym(dl, argv[i]);
        if (f == NULL) {
            dlclose(dl);
            continue;
        }
        x0 = f(x0);
        dlclose(dl);
    }
    printf("%.15Lg\n", x0);
}


