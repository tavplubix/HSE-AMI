#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


static size_t
readall(int fd, char *buf, size_t buf_size)
{
    size_t total_read = 0;
    ssize_t last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read <= 0) {
            return total_read;
        }
        total_read += last_read;
    } while (total_read != buf_size);
    return total_read;
}

static size_t
writeall(int fd, char *buf, size_t buf_size)
{
    size_t total_written = 0;
    ssize_t last_written;
    do {
        last_written = write(fd, buf + total_written, buf_size - total_written);
        if (last_written < 0) {
            return total_written;
        }
        total_written += last_written;
    } while (total_written != buf_size);
    return total_written;
}

static double
read_double(int fd)
{
    char buf[sizeof(double)];
    readall(fd, buf, sizeof(double));
    return *(double *) buf;
}

static void
write_double(int fd, double d)
{
    writeall(fd, (char *) &d, sizeof(double));
}

int
main(int argc, char **argv) {
    enum { FILENAME_IDX = 1, N_IDX = 2, BASE = 10 };
    long long N = strtoll(argv[N_IDX], NULL, BASE);
    int fd = open(argv[FILENAME_IDX], O_RDWR);
    off_t size = lseek(fd, 0, SEEK_END);
    if (size <= sizeof(double) || N <= 1) {
        close(fd);
        return 0;
    }
    if (size < N * sizeof(double)) {
        N = size / sizeof(double);
    }
    lseek(fd, 0, SEEK_SET);
    double first = read_double(fd);
    double second;
    for (long long i = 1; i < N; ++i) {
        second = read_double(fd);
        second -= first;
        lseek(fd, -(off_t) sizeof(double), SEEK_CUR);
        write_double(fd, second);
        first = second;
    }
    close(fd);
}


