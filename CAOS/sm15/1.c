#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int
main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    if (fstat(fd, &st) != 0) {
        exit(1);
    }
    size_t len = st.st_size / sizeof(double);
    double *arr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (arr == MAP_FAILED) {
        exit(1);
    }
    double sum = 0;
    for (size_t i = 0; i < len; ++i) {
        sum += arr[i];
    }

    printf("%a\n", sum / len);
    munmap(arr, st.st_size);
    close(fd);
}


