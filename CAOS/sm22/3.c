#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <sched.h>
#include <string.h>


long long *thread_sum;

void *
thread_func(void *_id)
{
    long id = (long) _id;
    int n;
    while (scanf("%d", &n) != EOF) {
        thread_sum[id] += n;
        sched_yield();
    }
    return NULL;
}

int
main(int argc, char **argv)
{
    long N = atol(argv[1]);
    thread_sum = malloc(N * sizeof(long long));
    memset(thread_sum, 0, N * sizeof(long long));
    pthread_t *threads = malloc(N * sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    pthread_attr_setguardsize(&attr, 0);
    for (long i = 0; i < N; ++i) {
        pthread_create(threads + i, &attr, thread_func, (void *) i);
    }
    long long sum = 0;
    for (long i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
        sum += thread_sum[i];
    }
    printf("%lld\n", sum);
    pthread_attr_destroy(&attr);
    free(threads);
    free(thread_sum);
}

