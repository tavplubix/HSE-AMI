#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int
run_and_wait(const char *cmd)
{
    pid_t child = fork();
    if (child < 0) {
        return 0;
    } else if (child == 0) {
        execlp(cmd, cmd, NULL);
        return 0;
    } else {
        int status;
        waitpid(child, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 1 : 0;
    }
}

int
main(int argc, char **argv)
{
    enum { CMD1 = 1, CMD2 = 2, CMD3 = 3 };
    return ((run_and_wait(argv[CMD1]) || run_and_wait(argv[CMD2])) 
            && run_and_wait(argv[CMD3])) ? 0 : 1;
}

