#include <stdio.h>

#include <dlfcn.h>

typedef double(*func_t)(double);

int
main(int argc, char **argv)
{
    void *handle = dlopen("/lib/libm.so.6", RTLD_LAZY);
    if (handle == NULL) {
        return 1;
    }
    func_t func = dlsym(handle, argv[1]);
    if (func == NULL) {
        return 1;
    }
    double d;
    while (scanf("%lf", &d) != EOF) {
        printf("%.10g\n", func(d));
    }
    dlclose(handle);
}


