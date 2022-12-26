#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>


int
main(int argc, char **argv)
{
    DIR *dir = opendir(argv[1]);
    int dfd = dirfd(dir);
    if (dir == NULL || dfd == -1) {
        return 1;
    }
    struct dirent *ent;
    long long size_sum = 0;
    while ((ent = readdir(dir)) != NULL) {
        if ('A' <= ent->d_name[0] && ent->d_name[0] <= 'Z') {
            struct stat stat;
            if (fstatat(dfd, ent->d_name, &stat, 0) == 0 
                && S_ISREG(stat.st_mode) && stat.st_uid == getuid()) {
                size_sum += stat.st_size;
            }
        }
    }
    closedir(dir);
    printf("%lld\n", size_sum);
}

