#include <stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        const char *file;
        int fd = open(argv[i], O_RDONLY);
        off_t size;
        if (fd == -1) {
            printf("-1\n");
        } else if ((size = lseek(fd, 0, SEEK_END)) == 0) {
            printf("0\n");
            close(fd);
        } else if ((file = mmap(NULL, size, PROT_READ, 
                    MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
            printf("-1\n");
            close(fd);
        } else {
            size_t lines = 1;
            for (off_t i = 0; i < size - 1; ++i) {
                if (file[i] == '\n') {
                    ++lines;
                }
            }
            printf("%zu\n", lines);
            munmap((void *) file, size);
            close(fd);
        }
    }
}


