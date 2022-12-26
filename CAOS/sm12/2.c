#include <unistd.h>
#include <stdlib.h>

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

void
copy_file(int in_fd, int out_fd)
{
    enum { BUF_SIZE = 4096 };
    char buf[BUF_SIZE];
    size_t last_copy;
    do {
        last_copy = readall(in_fd, buf, BUF_SIZE);
        if (last_copy != writeall(out_fd, buf, last_copy)) {
            exit(1);
        }
    } while (last_copy == BUF_SIZE);
}

