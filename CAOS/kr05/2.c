#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>
#include <inttypes.h>
#include <stdint.h>

enum { BUF_SIZE = 4, BLOCK_SIZE = 3 };

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

int32_t
unmarshall(unsigned char *buf)
{
    enum { MASK = 0x80, POS_BITS = 0x00, NEG_BITS = 0xFF };
    if (buf[1] & MASK) {
        buf[0] = NEG_BITS;
    } else {
        buf[0] = POS_BITS;
    }
    uint32_t le_res = be32toh(*(uint32_t *) buf);
    return *(int32_t *) &le_res;
}


int
main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    char buf[BUF_SIZE];
    int64_t sum = 0;
    while (readall(fd, buf + 1, BLOCK_SIZE) == BLOCK_SIZE) {
        sum += -unmarshall((unsigned char *) buf);
    }
    close(fd);
    printf("%"PRId64"\n", sum);
}


