#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>

enum { MAX_BUF_SIZE = 64 };

static void
prepare_args(const char *signature, char **argv,
                int first_idx, void *buf)
{
    int idx = 0;
    while (signature[idx + 1]) {
        int i;
        double d;
        switch (signature[idx + 1]) {
        case 'i':
            i = atoi(argv[first_idx + idx]);
            memcpy(buf, &i, sizeof(int));
            buf += sizeof(int);
            break;
        case 'd':
            d = atof(argv[first_idx + idx]);
            memcpy(buf, &d, sizeof(double));
            buf += sizeof(double);
            break;
        case 's':
            memcpy(buf, argv + first_idx + idx, sizeof(char *));
            buf += sizeof(char *);
            break;
        default:
            exit(1);
        }
        ++idx;
    }
}

static double
call_ret_st0(void *func, void *fargs)
{
    volatile double ret_val;
    __asm__ volatile    (
                        "mov    %2, %%eax   \n"
                        "mov    %3, %%ecx   \n"
                        "sub    %%ecx, %%esp\n"
                        "push_args_d:       \n"
                        "sub    $4, %%ecx   \n"
                        "mov    (%%eax, %%ecx,), %%edx  \n"
                        "mov    %%edx, (%%esp, %%ecx,)  \n"
                        "cmp    $0, %%ecx   \n"
                        "jg     push_args_d \n"
                        "mov    %1, %%ecx   \n"
                        "call   *%%ecx      \n"
                        "fstpl  (%%esp)     \n"
                        "mov    (%%esp), %%eax          \n"
                        "mov    %%eax, %0 \n"
                        "mov    4(%%esp), %%eax         \n"
                        "mov    %%eax, 4%0\n"
                        "mov    %3, %%eax   \n"
                        "add    %%eax, %%esp\n"
                        :
                        "=m" (ret_val)
                        :
                        "g" (func), "g" (fargs), "g" (MAX_BUF_SIZE)
                        :
                        "eax", "ecx", "edx", "esp"
                        );
    return ret_val;
}

static int __attribute__ ((noinline))
call_ret_eax(void *func, void *fargs)
{
    volatile int ret_val;
    __asm__ volatile    (
                        "mov    %2, %%eax   \n"
                        "mov    %3, %%ecx   \n"
                        "sub    %%ecx, %%esp\n"
                        "push_args_i:       \n"
                        "sub    $4, %%ecx   \n"
                        "mov    (%%eax, %%ecx,), %%edx  \n"
                        "mov    %%edx, (%%esp, %%ecx,)  \n"
                        "cmp    $0, %%ecx   \n"
                        "jg     push_args_i \n"
                        "mov    %1, %%ecx   \n"
                        "call   *%%ecx      \n"
                        "mov    %%eax, %0   \n"
                        "mov    %3, %%eax   \n"
                        "add    %%eax, %%esp\n"
                        :
                        "=g" (ret_val)
                        :
                        "g" (func), "g" (fargs), "g" (MAX_BUF_SIZE)
                        :
                        "eax", "ecx", "edx", "esp"
                        );
    return ret_val;
}

int
main(int argc, char **argv) {
    enum { DLIB_NAME_IDX = 1, FUNC_NAME_IDX = 2, 
            SIGNATURE_IDX = 3, ARGS_IDX = 4};
    void *dlib = dlopen(argv[DLIB_NAME_IDX], RTLD_LAZY);
    void *func = dlsym(dlib, argv[FUNC_NAME_IDX]);
    char fargs[MAX_BUF_SIZE];
    prepare_args(argv[SIGNATURE_IDX], argv, ARGS_IDX, fargs);
   
    union {
        double d;
        int i;
        char *s;
    } ret_val;
    switch (argv[SIGNATURE_IDX][0]) {
        case 'd':
            ret_val.d = call_ret_st0(func, fargs);
            printf("%.10g\n", ret_val.d);
            break;
        case 'i':
            ret_val.i = call_ret_eax(func, fargs);
            printf("%d\n", ret_val.i);
            break;
        case 's':
            ret_val.s = (char *) call_ret_eax(func, fargs);
            printf("%s\n", ret_val.s);
            break;
        case 'v':
            call_ret_eax(func, fargs);
            break;
        default:
            exit(1);
    }

    dlclose(dlib);
}


