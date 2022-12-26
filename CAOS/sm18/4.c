#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

static void
ping_pong(int childid, int infd, int outfd, long max_number)
{
    FILE *in = fdopen(infd, "r");
    FILE *out = fdopen(outfd, "w");
    setbuf(out, NULL);

    long num = 0;
    while (fscanf(in, "%ld", &num) != EOF && num != max_number) {
        printf("%d %ld\n", childid, num);
        fprintf(out, "%ld ", num + 1);
    }

    fclose(in);
    fclose(out);
}

int
main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    long max_number = atol(argv[1]);
    int pipe1[2];
    pipe(pipe1);
    int pipe2[2];
    pipe(pipe2);

    pid_t child1 = fork();
    if (child1 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        ping_pong(1, pipe1[0], pipe2[1], max_number);
        return 0;
    }
    close(pipe1[0]);
    close(pipe2[1]);
    pid_t child2 = fork();
    if (child2 == 0) {
        ping_pong(2, pipe2[0], pipe1[1], max_number);
    }
    close(pipe2[0]);
    FILE *child2_out = fdopen(pipe1[1], "w");
    fprintf(child2_out, "%ld ", 1l);
    fclose(child2_out);
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    printf("Done\n");
}

