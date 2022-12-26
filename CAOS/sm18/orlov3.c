#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    char *cmd1 = argv[1];
    char *cmd2 = argv[2];
    char *cmd3 = argv[3];
    char *file = argv[4];

    int wstatus;

    int fd[2];
    pipe(fd);

    pid_t son, cmd3_pid;

    if ((son = fork()) < 0) {}
    else if (!son) {
        close(fd[0]);

        pid_t cmd1_pid, cmd2_pid;

        if ((cmd1_pid = fork()) < 0) {}
        else if (!cmd1_pid) {
            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            execlp(cmd1, cmd1, NULL);
            _exit(0);
        }
        waitpid(cmd1_pid, &wstatus, 0);

        if ((cmd2_pid = fork()) < 0) {}
        else if (!cmd2_pid) {
            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            execlp(cmd2, cmd2, NULL);
            _exit(0);
        }
        close(fd[1]);
        waitpid(cmd2_pid, &wstatus, 0);
    }

    if ((cmd3_pid = fork()) < 0) {}
    else if (!cmd3_pid) {
        int file_des = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        dup2(fd[0], 0);
        dup2(file_des, 1);
        close(fd[1]);
        close(fd[0]);
        close(file_des);
        int ret = write(0, "qwerty\n", 7);
        execlp(cmd3, cmd3, NULL);
        exit(0);
    } else {
        waitpid(cmd3_pid, &wstatus, 0);
        close(fd[0]);
        close(fd[1]);
    }


    return 0;
}
