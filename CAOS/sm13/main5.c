struct Task
{
    unsigned uid;
    int gid_count;
    unsigned *gids;
};

#include "5.c"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main() {
    char buf[128];
    int perms;
    int uid;
    int cnt;
    unsigned gids[128];
    struct Task task;
    task.gids = gids;
    while (scanf("%s%o%d%d", buf, &perms, &uid, &cnt) != EOF) {
        task.uid = uid;
        task.gid_count = cnt;
        for (int i = 0; i < cnt; ++i)
            scanf("%u", task.gids + i);
        struct stat s;
        lstat(buf, &s);
        printf("%d %d\n", myaccess(&s, &task, perms), access(buf, perms));
    }
}


