#include <stdio.h>
#include <pthread.h>
#include <limits.h>


pthread_attr_t attr;

void *
thread_func(void *arg)
{
    int n;
    if (scanf("%d", &n) != EOF) {
        pthread_t id;
        pthread_create(&id, &attr, thread_func, NULL);
        pthread_join(id, NULL);
        printf("%d\n", n);
    }
    return NULL;
}

int
main()
{
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    pthread_t id;
    pthread_create(&id, &attr, thread_func, NULL);
    pthread_join(id, NULL);
    pthread_attr_destroy(&attr);
}

