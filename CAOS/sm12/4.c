#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


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

typedef struct Node
{
    int32_t key;
    int32_t left_idx;
    int32_t right_idx;
} Node;

static Node
read_node_by_index(int fd, int32_t index)
{
    Node n;
    off_t new_offset = lseek(fd, index * sizeof(Node), SEEK_SET);
    if (new_offset == (off_t) -1) {
        fputs("invalid file format\n", stderr);
        exit(1);
    }
    ssize_t bytes_read = readall(fd, (char *) &n, sizeof(Node));
    if (bytes_read != sizeof(Node)) {
        fputs("read error\n", stderr);
        exit(1);
    }
    return n;
}

static void
print_tree(int fd, int32_t root_index)
{
    Node n = read_node_by_index(fd, root_index);
    if (n.right_idx != 0) {
        print_tree(fd, n.right_idx);
    }
    printf("%"PRId32"\n", n.key);
    if (n.left_idx != 0) {
        print_tree(fd, n.left_idx);
    }
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fputs("few arguments\n", stderr);
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fputs("cannot open file\n", stderr);
        exit(1);
    }
    print_tree(fd, 0);
    close(fd);
}

