#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int
run_and_wait(int beg, char **argv)
{
    pid_t child = fork();
    if (child < 0) {
        return 1;
    } else if (child == 0) {
        execvp(argv[beg], argv + beg);
        _exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : 1;
    }
}

int
main(int argc, char **argv)
{
    int cmd_beg = 1;
    while (cmd_beg < argc) {
        int cmd_end = cmd_beg + 1;
        while (cmd_end < argc && strcmp("END", argv[cmd_end])) {
            ++cmd_end;
        }
        argv[cmd_end] = NULL;
        if (run_and_wait(cmd_beg, argv)) {
            _exit(1);
        }
        cmd_beg = cmd_end + 1;
    }
}

