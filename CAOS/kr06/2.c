#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
    enum { FILENAME_IDX = 1, COLS_IDX = 2, ROWS_IDX = 3 };
    int cols = atoi(argv[COLS_IDX]);
    int rows = atoi(argv[ROWS_IDX]);
    int fd = open(argv[FILENAME_IDX], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return 1;
    }
    off_t size = cols * rows * sizeof(double);
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return 1;
    }
    double *arr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (arr == MAP_FAILED) {
        close(fd);
        return 1;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            arr[i * cols + j] = 2 * sin((double) i / 3) + 3 * cos(j);
        }
    }

    munmap(arr, size);
    close(fd);
}


