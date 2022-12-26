#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void
kill_em_all(pid_t *childs, int n)
{
    for (int i = 0; i < n; ++i) {
        kill(childs[i], SIGKILL);
    }
    for (int i = 0; i < n; ++i) {
        waitpid(childs[i], NULL, 0);
    }
    _exit(1);
}

int
main(int argc, char **argv)
{
    pid_t *childs = malloc((argc - 1) * sizeof(pid_t));
    int infd = 0;
    for (int i = 1; i < argc - 1; ++i) {
        int pipefd[2];
        if (pipe(pipefd)) {
            kill_em_all(childs, i - 1);
        }
        int outfd = pipefd[1];
        childs[i - 1] = fork();
        if (childs[i - 1] < 0) {
            kill_em_all(childs, i - 1);
        } else if (childs[i - 1] == 0) {
            close(pipefd[0]);
            if (infd) {
                dup2(infd, 0);
                close(infd);
            }
            dup2(outfd, 1);
            close(outfd);
            execlp(argv[i], argv[i], NULL);
        }
        int closeret = close(outfd);
        if (infd) {
            closeret |= close(infd);
        }
        if (closeret) {
            kill_em_all(childs, i);
        }
        infd = pipefd[0];
    }
    if (argc > 1) {
        childs[argc - 2] = fork();
        if (childs[argc - 1] < 0) {
            kill_em_all(childs, argc - 2);
        } else if (childs[argc - 2]) {
            if (infd) {
                dup2(infd, 0);
                close(infd);
            }
            execlp(argv[argc - 1], argv[argc - 1], NULL);
        }
    }
    if (infd) {
        if (close(infd)) {
            kill_em_all(childs, argc - 1);
        }
    }
    
    for (int i = 1; i < argc; ++i) {
        waitpid(childs[i - 1], NULL, 0);
    }
}

