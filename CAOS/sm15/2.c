#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static long rows, cols;
static uint32_t *arr;

static uint32_t *
at(long x, long y)
{
    return arr + (y * cols + x);
}


static void
fill_mat(long x_beg, long y_beg, long x_end, long y_end, uint32_t val)
{
    if (x_beg >= x_end || y_beg >= y_end) {
        return;
    }
    for (long x = x_beg; x < x_end; ++x) {
        *at(x, y_beg) = val++;
    }
    for (long y = y_beg + 1; y < y_end; ++y) {
        *at(x_end - 1, y) = val++;
    }
    if (y_end - y_beg > 1) {
        for (long x = x_end - 2; x >= x_beg; --x) {
            *at(x, y_end - 1) = val++;
        }
    }
    if (x_end - x_beg > 1) {
        for (long y = y_end - 2; y > y_beg; --y) {
            *at(x_beg, y) = val++;
        }
    }
    fill_mat(x_beg + 1, y_beg + 1, x_end - 1, y_end - 1, val);
}


int
main(int argc, char **argv)
{
    rows = atol(argv[2]);
    cols = atol(argv[3]);
    int fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    size_t size = rows * cols;
    if (size) {
        lseek(fd, size * sizeof(uint32_t) - 1, SEEK_SET);
        write(fd, "", 1);
    }
    //close(fd);
    //fd = open(argv[1], O_RDWR);
    arr = mmap(NULL, size * sizeof(uint32_t), PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);
    fill_mat(0, 0, cols, rows, 1);

    munmap(arr, size * sizeof(uint32_t));
    close(fd);
}


