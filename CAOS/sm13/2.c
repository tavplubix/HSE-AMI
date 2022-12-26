#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

int
main(int argc, char **argv)
{
    uint64_t size_sum = 0;
    struct stat st;
    for (int i = 1; i < argc; ++i) {
        if (lstat(argv[i], &st) != 0) {
            continue;
        }
        if (st.st_nlink == 1 && S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode)) {
            size_sum += st.st_size;
        }
    }
    printf("%"PRIu64"\n", size_sum);
}

