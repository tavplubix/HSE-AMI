#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>
#include <sys/wait.h>

void make_temp_file(char* name, char* func) {
    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0777);
    char* beg = "\
#include <math.h>\n\
#include <stdio.h>\n\
double func(double x) {\n\
    return ";
    char* end = ";\n}";
    size_t size_arr = strlen(beg) + strlen(func) + strlen(end);
    ftruncate(fd, size_arr);
    void *ptr = mmap(0, size_arr, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char *arr = ptr;
    strcpy(arr, beg);
    arr += strlen(beg);
    strcpy(arr, func);
    arr += strlen(func);
    strcpy(arr, end);
    arr += strlen(end);
    munmap(ptr, size_arr);
    close(fd);
}

double func_comp(double dx, double p_left, char* function, void* sym) {
    double res = ((double (*)(double)) sym)(p_left);
    return (dx * res);
}

int main(int argc, char** argv) {
    if (argc < 5) {
        return 0;
    }
    char* func = argv[4];
    char* name = "sowdjfdpokdvpsv.c";
    make_temp_file(name, func);
    double left, right;
    left = atof(argv[1]);
    right = atof(argv[2]);
    int n = atoi(argv[3]);
    double dx = (right - left) * 1.0 / n;
    int pid = fork();
    if (!pid) {
        execlp("gcc", "gcc", "-shared", "-lm", name, "-o", "fakelib.so", NULL);
        _exit(1);
    }
    waitpid(pid, NULL, 0);
    void* handle = dlopen("./fakelib.so", RTLD_LAZY);
    if (dlerror() != NULL) {
        return 0;
    }
    void* sym = dlsym(handle, "func");
    if (dlerror() != NULL) {
        return 0;
    }
    double result = 0;
    for (size_t i = 0; i < n; ++i) {
        result += func_comp(dx, left + i * dx, func, sym);
    }
    dlclose(handle);
    printf("%.10g\n", result);
    fflush(stdout);
}
