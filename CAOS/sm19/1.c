#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static volatile int sigcnt = 0;

static void
handler(int signum)
{
    enum { MAX_SIG_CNT = 4 };
    if (sigcnt == MAX_SIG_CNT) {
        exit(0);
    }
    printf("%d\n", sigcnt++);
    fflush(stdout);
}

int
main()
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = handler;
    sigaction(SIGINT, &sigact, NULL);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    printf("%d\n", getpid());
    fflush(stdout);
    while(1) {
        sigsuspend(&oldmask);
    }
}

