#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

static int
run_and_wait(const char *cmd, int infd, int outfd)
{
    pid_t child = fork();
    if (child < 0) {
        return 1;
    } else if (child == 0) {
        dup2(infd, 0);
        close(infd);
        dup2(outfd, 1);
        dup2(outfd, 2);
        close(outfd);
        execlp(cmd, cmd, NULL);
        _exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            close(infd);
            close(outfd);
            return 0;
        }
        return 1;
    }
}

int
main(int argc, char **argv) {
    int pipefd[2];
    pipe(pipefd);
    pid_t cmd2 = fork();
    if (cmd2 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        execlp(argv[2], argv[2], NULL);
        _exit(1);
    }
    close(pipefd[1]);

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0660);
    int ret = !run_and_wait(argv[3], pipefd[0], fd) 
    || !run_and_wait(argv[4], pipefd[0], fd);

    return ret / 1000;
}

