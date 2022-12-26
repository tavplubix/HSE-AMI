#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static volatile sig_atomic_t mode = 0;

static void
handler(int signum)
{
    if (signum == SIGUSR1) {
        mode = 0;
    } else if (signum == SIGUSR2) {
        mode = 1;
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


    printf("%d\n", getpid());
    fflush(stdout);
    
    long long num;
    while(scanf("%lld", &num) != EOF) {
        if (mode == 0) {
            num = -num;
        } else {
            num *= num;
        }
        printf("%lld\n", num);
        fflush(stdout);
    }
}

