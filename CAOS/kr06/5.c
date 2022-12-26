#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    int64_t max_sum_size = atoll(argv[2]);
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        return 1;
    }
    struct dirent *ent;
    int64_t size_sum = 0;
    size_t len = strlen(argv[1]);
    if (len == 0 || PATH_MAX <= len + 1) {
        return 1;
    }
    char buf[PATH_MAX];
    strcpy(buf, argv[1]);
    if (len == 0 || buf[len - 1] != '/') {
        buf[len++] = '/';
    }
    char filename[NAME_MAX];
    int64_t max_file_size = 0;
    while ((ent = readdir(dir)) != NULL) {
        strncpy(buf + len, ent->d_name, PATH_MAX - len);
        struct stat st;
        if (lstat(buf, &st) == 0 && S_ISREG(st.st_mode)) {
            size_sum += st.st_size;
            if (max_file_size < st.st_size || 
                (max_file_size == st.st_size 
                && strcmp(filename, ent->d_name) > 0))
            {
                max_file_size = st.st_size;
                strcpy(filename, ent->d_name);
            }
        }
    }
    closedir(dir);
    if (max_sum_size < size_sum) {
        printf("%s\n", filename);
    }
}

