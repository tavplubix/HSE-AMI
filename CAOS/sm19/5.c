#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <signal.h>


static size_t
readall(int fd, char *buf, size_t buf_size)
{
    size_t total_read = 0;
    ssize_t last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read <= 0) {
            return total_read;
        }
        total_read += last_read;
    } while (total_read != buf_size);
    return total_read;
}

static void
ping_pong(int id, pid_t bro, int infd, int outfd, int N)
{
    FILE *in = fdopen(infd, "r");
    FILE *out = fdopen(outfd, "w");

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    int sigfd = signalfd(-1, &mask, 0);

    if (id == 1) { 
        fscanf(in, "%d", &bro);
        
    } else {
        fprintf(out, "%d\n%d\n", getpid(), 1);
        fflush(out);
        kill(bro, SIGUSR1);
    }

    int n = 0;
    while(n < N) {
        struct signalfd_siginfo siginfo;
        readall(sigfd, (void *) &siginfo, sizeof(siginfo));
        if (siginfo.ssi_signo != SIGUSR1 || siginfo.ssi_pid != bro) {
            continue;
        }
        fscanf(in, "%d", &n);
        if (n > N) {
            break;
        }
        fprintf(out, "%d\n", n + 1);
        fflush(out);
        printf("%d %d\n", id, n);
        fflush(stdout);
        kill(bro, SIGUSR1);
    }

    fclose(in);
    fclose(out);
    close(sigfd);
    exit(0);
}

int
main(int argc, char **argv)
{
    int N = atoi(argv[1]);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    int id = 1;
    int pipefd[2];
    pipe(pipefd);
    pid_t child1 = fork();
    if (child1 == 0) {
        ping_pong(id, 0, pipefd[0], pipefd[1], N);
    }
    ++id;
    pid_t child2 = fork();
    if (child2 == 0) {
        ping_pong(id, child1, pipefd[0], pipefd[1], N);
    }
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
}

