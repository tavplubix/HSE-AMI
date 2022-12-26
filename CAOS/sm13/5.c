#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>


struct Task
{
    unsigned uid;
    int gid_count;
    unsigned *gids;
};

static bool
has_gid(const struct Task *task, gid_t gid)
{
    for (int i = 0; i < task->gid_count; ++i) {
        if (task->gids[i] == gid) {
            return true;
        }
    }
    return false;
}


int
myaccess(const struct stat *stb, const struct Task *task, int access)
{
    if (task->uid == 0) {
        return 1;
    }
    enum { USR_SHIFT = 6, GRP_SHIFT = 3, OTH_SHIFT = 0 };
    if (stb->st_uid == task->uid) {
        return ((stb->st_mode >> USR_SHIFT) & access) == access;
    }
    if (has_gid(task, stb->st_gid)) {
        return ((stb->st_mode >> GRP_SHIFT) & access) == access;
    }
    return ((stb->st_mode >> OTH_SHIFT) & access) == access;
}

