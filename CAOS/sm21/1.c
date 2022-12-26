#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void
handler(int signum)
{
    _exit(0);
}

int
main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(SIGALRM, &sa, NULL);

    time_t sec;
    int usec;
    scanf("%ld%d", &sec, &usec);
    usec /= 1000;
    struct timeval now;
    gettimeofday(&now, NULL);
    sec -= now.tv_sec;
    usec -= now.tv_usec;
    if (usec < 0) {
        enum { MAX_USEC = 1000000 };
        usec += MAX_USEC;
        sec -= 1;
    }
    if (sec < 0) {
        return 0;
    }

    struct itimerval t;
    memset(&t, 0, sizeof(t));
    t.it_value.tv_sec = sec;
    t.it_value.tv_usec = usec;
    setitimer(ITIMER_REAL, &t, NULL);
    pause();
}

