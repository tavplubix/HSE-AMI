#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct stat st;
    lstat(argv[1], &st);
    printf("%d %d\n", S_ISREG(st.st_mode), S_ISLNK(st.st_mode));

    char buf[4097];
    buf[0] = ' ';
    ssize_t s = readlink(argv[1], buf + 1, 4096);
    buf[s + 1] = 0;
    printf("%d %s\n", s, buf);

}

