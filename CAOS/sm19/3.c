#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


static volatile sig_atomic_t usr1_cnt = 0;
static volatile sig_atomic_t usr2_cnt = 0;
static volatile sig_atomic_t print_cnt = 0;
static volatile sig_atomic_t term = 0;

static void
handler(int signum)
{
    if (signum == SIGUSR1) {
        ++usr1_cnt;
        print_cnt = 1;
    } else if (signum == SIGUSR2) {
        ++usr2_cnt;
    } else if (signum == SIGTERM) {
        term = 1;
    }
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
    sigaction(SIGTERM, &sigact, NULL);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);

    printf("%d\n", getpid());
    fflush(stdout);

    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while(1) {
        sigsuspend(&oldmask);
        if (print_cnt == 1) {
            printf("%d %d\n", usr1_cnt - 1, usr2_cnt);
            fflush(stdout);
            print_cnt = 0;
        }
        if (term == 1) {
            exit(0);
        }
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

