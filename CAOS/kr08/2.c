#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void
process(int id, int infd, int outfd, int N)
{    
    int n = 0;
    while (read(infd, &n, sizeof(n)) && n <= N) {
        if (n <= N) {
            printf("%d %d\n", id, n);
        }
        ++n;
        write(outfd, &n, sizeof(n));
    }
    close(infd);
    close(outfd);
}

int
main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    int N = atoi(argv[1]);
    int pipe1to3[2];
    pipe(pipe1to3);
    int pipe2to1[2];
    pipe(pipe2to1);
    pid_t child = fork();
    if (child == 0) {
        close(pipe1to3[1]);
        close(pipe2to1[0]);
        int pipe3to2[2];
        pipe(pipe3to2);
        pid_t child = fork();
        if (child == 0) {
            close(pipe3to2[0]);
            close(pipe2to1[1]);
            process(3, pipe1to3[0], pipe3to2[1], N);
            return 0;
        }
        close(pipe3to2[1]);
        close(pipe1to3[0]);
        process(2, pipe3to2[0], pipe2to1[1], N);
        waitpid(child, NULL, 0);
        return 0;
    }
    close(pipe1to3[0]);
    close(pipe2to1[1]);
    
    int n = 1;
    write(pipe1to3[1], &n, sizeof(n));
    process(1, pipe2to1[0], pipe1to3[1], N);
    waitpid(child, NULL, 0);
}

