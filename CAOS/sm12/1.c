#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

int
main(int argc, char **argv)
{
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    unsigned short val;
    while (scanf("%hu", &val) != EOF) {
        enum { BUF_SIZE = 2 };
        unsigned char buf[BUF_SIZE];
        enum { MASK = 0xFF };
        buf[1] = val & MASK;
        buf[0] = val >> CHAR_BIT;
        write(fd, buf, BUF_SIZE);
    }
    close(fd);
}





