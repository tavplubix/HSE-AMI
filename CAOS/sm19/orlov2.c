#include <stdio.h>
#include <memory.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>
/*
#define NEGATE ((sig_atomic_t) 0)
#define SQUARE ((sig_atomic_t) 1)

volatile sig_atomic_t state = NEGATE;
*/
/*
void sigusr_handler(int signum, siginfo_t *info, void *usercontext) {
    if (signum == SIGUSR1) {
        state = NEGATE;
    } else if (signum == SIGUSR2) {
        state = SQUARE;
    }
}

void set_signals() {
    struct sigaction act = {};
    act.sa_sigaction = sigusr_handler;
    act.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        exit(0);
    }
    if (sigaction(SIGUSR2, &act, NULL) < 0) {
        exit(0);
    }
}
*/

int main() {
    // setbuf(stdin, NULL);
    // set_signals();
    // sigset_t mask = {}, oldmask = {};
    // sigemptyset(&mask);
    // sigaddset(&mask, SIGUSR1);
    // sigaddset(&mask, SIGUSR2);

    // printf("%d\n", getpid());

    int x;
    while (1) {
        int result = scanf("%d ", &x);
        if (result == EOF) {
            return 0;
        }
        // sigprocmask(SIG_BLOCK, &mask, &oldmask);
        //if (state == NEGATE) {
            printf("%d\n", -x);
            fflush(stdout);
        //} else if (state == SQUARE) {
        //    printf("%d\n", x * x);
        //    fflush(stdout);
        //}
        // sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
    }
}
