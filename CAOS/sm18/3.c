#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int
main(int argc, char **argv)
{
    int pipefd[2];
    pipe(pipefd);
    pid_t cmd3 = fork();
    if (cmd3 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        close(pipefd[0]);
        int fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        dup2(fd, 1);
        close(fd);
        execlp(argv[3], argv[3], NULL);
        _exit(1);
    }
    close(pipefd[0]);
    pid_t cmd1 = fork();
    if (cmd1 == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        execlp(argv[1], argv[1], NULL);
        _exit(1);
    }
    waitpid(cmd1, NULL, 0);
    pid_t cmd2 = fork();
    if (cmd2 == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        execlp(argv[2], argv[2], NULL);
        _exit(1);
    }
    close(pipefd[1]);
    waitpid(cmd2, NULL, 0);
    waitpid(cmd3, NULL, 0);
}

