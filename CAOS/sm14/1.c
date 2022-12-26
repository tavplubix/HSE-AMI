#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>


int
main(int argc, char **argv)
{
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        return 1;
    }
    struct dirent *ent;
    long long size_sum = 0;
    size_t len = strlen(argv[1]);
    if (len == 0 || PATH_MAX <= len + 1) {
        return 1;
    }
    char buf[PATH_MAX];
    strcpy(buf, argv[1]);
    if (buf[len - 1] != '/') {
        buf[len++] = '/';
    }
    while ((ent = readdir(dir)) != NULL) {
        strncpy(buf + len, ent->d_name, PATH_MAX - len);
        struct stat st;
        if (stat(buf, &st) == 0 
            && S_ISREG(st.st_mode) && st.st_uid == getuid()
            && 'A' <= ent->d_name[0] && ent->d_name[0] <= 'Z') {
            size_sum += st.st_size;
        }
    }
    closedir(dir);
    printf("%lld\n", size_sum);
}

