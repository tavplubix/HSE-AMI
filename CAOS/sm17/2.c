#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
mysys(const char *cmd)
{
    enum { MAX_RET_VAL = 127 };
    pid_t child = fork();
    if (child < 0) {
        return -1;
    } else if (child == 0) {
        execlp("/bin/sh", "sh", "-c", cmd, NULL);
        _exit(MAX_RET_VAL);
    } else {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
            return status;
        } else if (WIFSIGNALED(status)) {
            return MAX_RET_VAL + 1 + WTERMSIG(status);
        } else {
            return -1;
        }
    }
}

