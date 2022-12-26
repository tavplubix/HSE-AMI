#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int count = 0;

static int
run_and_wait(char *cmd)
{
    pid_t child = fork();
    if (child < 0) {
        return 1;
    } else if (child == 0) {
        execlp(cmd, cmd, NULL);
        _exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            ++count;
            return 0;
        }
        return 1;
    }
}

static int
get_result(pid_t child) {
    int status;
    waitpid(child, &status, 0);
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 1 : 0;
}

int
main(int argc, char **argv)
{
    pid_t *childs = malloc((argc - 3) * sizeof(pid_t));
    while (!run_and_wait(argv[1]) || !run_and_wait(argv[2])) {
        for (int i = 3; i < argc; ++i) {
            childs[i - 3] = fork();
            if (childs[i - 3] == 0) {
                execlp(argv[i], argv[i], NULL);
                _exit(1);
            }
        }
        for (int i = 3; i < argc; ++i) {
            if (childs[i - 3] > 0) {
                count += get_result(childs[i - 3]);
            }
        }
    }
    free(childs);
    printf("%d\n", count);
}

