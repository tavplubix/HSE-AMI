#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>


int
main(int argc, char **argv)
{
    enum { FORMAT_IDX = 1, LOW_IDX = 2, HIGH_IDX = 3, ARG_CNT = 4 };
    if (argc < ARG_CNT) {
        return 1;
    }
    enum { BASE = 10 };
    int32_t low = strtol(argv[LOW_IDX], NULL, BASE);
    int32_t high = strtol(argv[HIGH_IDX], NULL, BASE);
    enum { MAX_INT_LEN = 16 };
    size_t buf_size = strlen(argv[FORMAT_IDX]) + MAX_INT_LEN;
    char *buf = malloc(buf_size);
    uint64_t size_sum = 0;
    struct stat st;
    for (int32_t i = low; i <= high; ++i) {
        snprintf(buf, buf_size, argv[FORMAT_IDX], i);
        if (lstat(buf, &st) != 0) {
            continue;
        }
        if (S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode) 
            && st.st_uid == getuid()) {
            size_sum += st.st_size;
        }
    }
    free(buf);
    printf("%"PRIu64"\n", size_sum);
}


