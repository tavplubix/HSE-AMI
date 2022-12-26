#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

enum { ERROR = 1, IO_ERROR = 2, ARITHMETIC_ERROR = 3 };

static size_t
readall(int fd, char *buf, size_t buf_size)
{
    size_t total_read = 0;
    ssize_t last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read <= 0) {
            fputs("error while reading\n", stderr);
            exit(IO_ERROR);
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
            fputs("error while writing\n", stderr);
            exit(IO_ERROR);
        }
        total_written += last_written;
    } while (total_written != buf_size);
    return total_written;
}

struct Data
{
    int16_t x; 
    int64_t y;
}  __attribute__((packed));

static void
marshall(char *out, const struct Data *in)
{
    memcpy(out, in, sizeof(struct Data));
    *(uint16_t *) out = htole16(*(uint16_t *) out);
    *(uint64_t *) (out + sizeof(uint16_t)) = 
                    htole64(*(uint64_t *) (out + sizeof(uint16_t)));
}

static void
unmarshall(struct Data *out, const char *in)
{
    memcpy(out, in, sizeof(struct Data));
    *(uint16_t *) out = le16toh(*(uint16_t *) out);
    uint64_t *yptr = ((void *) out) + sizeof(uint16_t);
    *yptr = le64toh(*yptr);
}

static void
process_struct(struct Data *data, int32_t A)
{
    int64_t add = (int64_t) data->x * (int64_t) A;
    if (__builtin_add_overflow(data->y, add, &data->y)) {
        fputs("integer overflow\n", stderr);
        exit(ARITHMETIC_ERROR);
    }
}

static void
set_offset(int fd, off_t offset)
{
    off_t new_offset = lseek(fd, offset, SEEK_SET);
    if (new_offset < 0) {
        fputs("invalid file format\n", stderr);
        exit(IO_ERROR);
    }

}

static void
process_file(int fd, off_t size, int32_t A)
{
    set_offset(fd, 0);
    off_t begin = 0;
    off_t end = size - sizeof(struct Data);
    char buf[sizeof(struct Data)];
    struct Data b_data, e_data;
    while (begin < end) {
        readall(fd, buf, sizeof(struct Data));
        unmarshall(&b_data, buf);
        process_struct(&b_data, A);

        set_offset(fd, end);
        readall(fd, buf, sizeof(struct Data));
        unmarshall(&e_data, buf);
        process_struct(&e_data, A);

        set_offset(fd, end);
        marshall(buf, &b_data);
        writeall(fd, buf, sizeof(struct Data));

        set_offset(fd, begin);
        marshall(buf, &e_data);
        writeall(fd, buf, sizeof(struct Data));

        begin += sizeof(struct Data);
        end -= sizeof(struct Data);
    }
    if (begin == end) {
        readall(fd, buf, sizeof(struct Data));
        unmarshall(&b_data, buf);
        process_struct(&b_data, A);
        marshall(buf, &b_data);
        set_offset(fd, begin);
        writeall(fd, buf, sizeof(struct Data));
    }
}


int
main(int argc, char **argv)
{
    if (argc < 3) {
        fputs("few arguments\n", stderr);
        exit(ERROR);
    }
    char *endptr;
    errno = 0;
    int32_t A = strtol(argv[2], &endptr, 10);
    if (errno || argv[2][0] == '\0' || *endptr != '\0') {
        fputs("invalid arguments\n", stderr);
        exit(ERROR);
    }
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        fputs("cannot open file\n", stderr);
        exit(IO_ERROR);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size % sizeof(struct Data) != 0) {
        fputs("invalid file format\n", stderr);
    }
    
    process_file(fd, file_size, A);

    if (close(fd) != 0) {
        fputs("cannot close file\n", stderr);
        exit(IO_ERROR);
    }
}

