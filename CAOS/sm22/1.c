#include <stdio.h>
#include <pthread.h>


enum { THREAD_COUNT = 10 };

pthread_t threads[THREAD_COUNT];

void *
thread_func(void *_id)
{
    long id = (long) _id;
    if (id) {
        pthread_join(threads[id - 1], NULL);
    }
    printf("%ld\n", id);
    return NULL;
}

int
main()
{
    for (long i = 0; i < THREAD_COUNT; ++i) {
        pthread_create(threads + i, NULL, thread_func, (void *) i);
    }
    pthread_join(threads[THREAD_COUNT - 1], NULL);
}

