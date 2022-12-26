#include <sys/types.h>
struct FileContent
{
    ssize_t size;
    char *data;
};

#include "3.c"

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char **argv) {
    int fd = open(argv[1], O_RDONLY);
    struct FileContent fc = read_file(fd);
    printf("%ld\n", fc.size);
    if (fc.size >= 0) {
        printf("%s", fc.data);
    }
}


