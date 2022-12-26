#include <stdio.h>
#include <sys/stat.h>
struct Task
{
    unsigned uid;
    int gid_count;
    unsigned *gids;
};

extern
int myaccess(const struct stat *stb, const struct Task *task, int access);

int main() {
    struct Task k;
    struct stat s;
    k.uid = 4;
    s.st_uid = 4;
    k.gid_count = 2;
    unsigned int a[2] = {1, 2};
    k.gids = a;
    s.st_mode = 0;
    s.st_gid = 2;
    printf("uid == uid\n");
    for (int j = 0; j < 1 << 8; ++j) {
        s.st_mode = j;
        for (int i = 0; i != 8; ++i) {
            printf("%d , %d  =  %d\n", (s.st_mode), i, myaccess(&s, &k, i));
        }
    }
    printf("uid != uid, guid exist in guidlist\n");
    for (int j = 0; j < 1 << 8; ++j) {
        s.st_mode = j;
        for (int i = 0; i != 8; ++i) {
            printf("%d , %d  =  %d\n", (s.st_mode), i, myaccess(&s, &k, i));
        }
    }
    printf("uid != uid, guid != guid\n");
    s.st_gid = 3;
    for (int j = 0; j < 1 << 8; ++j) {
        s.st_mode = j;
        for (int i = 0; i != 8; ++i) {
            printf("%d , %d  =  %d\n", (s.st_mode), i, myaccess(&s, &k, i));
        }
    }
}

