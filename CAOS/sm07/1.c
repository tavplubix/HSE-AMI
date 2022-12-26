#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>


enum { MAX_FUNC_NAME_LEN = 8 };

typedef double (*funcptr_t)(double);

typedef struct FunctionTable
{
    const char *fname;
    funcptr_t fptr;
} FunctionTable;

static FunctionTable ftab[] = {
    { "sin", sin },
    { "cos", cos },
    { "exp", exp },
    { "log", log },
    { "tan", tan },
    { "sqrt", sqrt }
};

bool
call_func_from_table(const char *fname, double arg, double *res)
{
    for (size_t i = 0; i < sizeof(ftab) / sizeof(FunctionTable); ++i) {
        if (strcmp(fname, ftab[i].fname) == 0) {
            *res = (*ftab[i].fptr)(arg);
            return true;
        }
    }
    return false;
}

bool
read_func_name(char *buf)
{
    int cur_c = ' ';
    while (isspace(cur_c = fgetc(stdin))) {}
    size_t name_len = 0;
    while (!isspace(cur_c) && cur_c != EOF) {
        if (name_len < MAX_FUNC_NAME_LEN) {
            *buf++ = cur_c;
            ++name_len;
        }
        cur_c = fgetc(stdin);
    }
    *buf = '\0';
    return name_len <= MAX_FUNC_NAME_LEN;
}

int
main()
{
    char buf[MAX_FUNC_NAME_LEN + 1];
    bool valid_name = read_func_name(buf);
    double arg;
    while (scanf("%lf", &arg) != EOF) {
        double res;
        if (valid_name && call_func_from_table(buf, arg, &res)) {
            printf("%a\n", res);
        } else {
            fputs("nan\n", stdout);
        }
        valid_name = read_func_name(buf);
    }
}


