#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


static ssize_t
readall(int fd, char *buf, ssize_t buf_size)
{
    ssize_t total_read = 0;
    ssize_t last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read == 0) {
            return total_read;
        }
        if (last_read < 0) {
            return last_read;
        }
        total_read += last_read;
    } while (total_read != buf_size);
    return total_read;
}

struct FileContent
read_file(int fd)
{
    enum { BUF_SIZE = 4096 };
    char buf[BUF_SIZE];
    struct FileContent fc = { 1, NULL };
    ssize_t last_read;
    do {
        last_read = readall(fd, buf, BUF_SIZE);
        if (last_read < 0) {
            goto error;
        }
        char *new_data = realloc(fc.data, fc.size + last_read);
        if (new_data == NULL) {
            goto error;
        }
        fc.data = new_data;
        memcpy(fc.data + fc.size - 1, buf, last_read);
        fc.size += last_read;
    } while (last_read == BUF_SIZE);
    --fc.size;
    fc.data[fc.size] = '\0';
    return fc;
    
    error:
    free(fc.data);
    fc.size = -1;
    fc.data = NULL;
    return fc;
}

