#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static volatile sig_atomic_t cnt = 0;
static volatile sig_atomic_t term = 0;
static volatile sig_atomic_t lastsignum;

static void
handler(int signum)
{
    if (signum == SIGUSR1) {
        cnt += 5;
    } else if (signum == SIGUSR2) {
        cnt -= 4;
    }
    if (cnt < 0) {
        term = 1;
    }
    lastsignum = signum;
}

int
main()
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = handler;
    sigact.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sigact, NULL);
    sigaction(SIGUSR2, &sigact, NULL);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    printf("%d\n", getpid());
    fflush(stdout);
    
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while(1) {
        sigsuspend(&oldmask);
        printf("%d %d\n", lastsignum, cnt);
        fflush(stdout);
        if (term == 1) {
            exit(0);
        }
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

