#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

enum { N = 6 };

pthread_t ids[N];
pthread_t ids2[N];

void *func2(void *arg)
{
    pause();
    return NULL;
}

int wait_for[N] = { 1, 3, 4, 5, 0, -1};
int next_thread[N] = { 4, 0, -1, 1, 2, 3 };
enum { FIRST_THREAD = 5, LAST_THREAD = 2 };

void
handler(int signum) {    
}



void *func(void *arg)
{
    int serial = (int) (intptr_t) arg;
    pthread_join(ids2[serial], NULL);
    if (wait_for[serial] != -1) {
        pthread_join(ids[wait_for[serial]], NULL);
    }
    printf("%d\n", serial);
    if (next_thread[serial] != -1) {
        pthread_kill(ids2[next_thread[serial]], SIGUSR1);
    }
    return NULL;
}

int main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);
    for (int i = 0; i < N; ++i) {
        pthread_create(&ids2[i], NULL, func2, NULL);
    }
    for (int i = 0; i < N; ++i) {
        pthread_create(&ids[i], NULL, func, (void*) (intptr_t) i);
    }
    
    pthread_kill(ids2[FIRST_THREAD], SIGUSR1);
    pthread_join(ids[LAST_THREAD], NULL);
    return 0;
}
