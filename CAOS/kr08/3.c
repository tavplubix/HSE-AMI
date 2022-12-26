#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef double (*func_t)(double);

static const char prog1[] =     "#include <stdio.h>\n"
                                "#include <math.h>\n"
                                "double func(double x) {\n"
                                "   return ";
static const char prog2[] =                           ";\n"
                                "}\n";

static double
integral(double left, double right, long long n, func_t f)
{
    double sum = 0;
    double step = (right - left) / n;
    for (long long i = 0; i < n; ++i) {
        sum += f(left);
        left += step;
    }
    return sum * step;
}

int
main(int argc, char **argv)
{
    int fd = open("temp.c", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(fd, prog1, sizeof(prog1) - 1);
    write(fd, argv[4], strlen(argv[4]));
    write(fd, prog2, sizeof(prog2) - 1);
    close(fd);
    pid_t gcc = fork();
    if (gcc == 0) {
        execlp("gcc", "gcc", "-shared", "-lm", "temp.c", 
                                            "-o", "libtemp.so", NULL);
        _exit(1);
    }
    double left = strtod(argv[1], NULL);
    double right = strtod(argv[2], NULL);
    long long n = strtoll(argv[3], NULL, 10);
    waitpid(gcc, NULL, 0);
    unlink("temp.c");
 
    void *dl = dlopen("./libtemp.so", RTLD_LAZY);
    if (dl == NULL) {
        return 0;
    }
    func_t f = dlsym(dl, "func");
    printf("%.10g\n", integral(left, right, n, f));
    dlclose(dl);
    unlink("libtemp.so");
}

