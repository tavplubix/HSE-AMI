#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int
cmp_d(const void *a, const void *b)
{
    double da = *(double *) a;
    double db = *(double *) b;
    if (da < db) {
        return -1;
    } else if (da > db) {
        return 1;
    } else {
        return 0;
    }
}

int
main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    off_t size = st.st_size;
    if (size == 0) {
        printf("0\n");
        close(fd);
        return 0;
    }

    enum { SKIP_PART = 10 };
    off_t offset = (size / (SKIP_PART * sizeof(double))) * sizeof(double);
    double *arr = mmap(NULL, st.st_size - offset, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE, fd, 0);
    arr += offset / sizeof(double);
    size -= 2 * offset;
    size /= sizeof(double);
    offset = size / SKIP_PART;

    qsort(arr, size, sizeof(double), cmp_d);

    double sum = 0;
    for (off_t i = offset; i < size - offset; ++i) {
        sum += arr[i];
    }

    printf("%.10g\n", sum / (size - 2 * offset));

    munmap(arr, st.st_size);
    close(fd);
}

