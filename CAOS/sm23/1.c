#include <stdio.h>
#include <pthread.h>
#include <string.h>

enum { THREADS = 3, ITERATIONS = 1000000 };

double data[THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
func(void *_id)
{
    int id = (int) _id;
    for (int i = 0; i < ITERATIONS; ++i) {
        pthread_mutex_lock(&mutex);
        data[id] += (id + 1) * 100;
        data[(id + 1) % THREADS] -= (id + 1) * 100 + 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int
main()
{
    memset(data, 0, sizeof(data));
    pthread_t threads[THREADS];
    for (int i = 0; i < THREADS; ++i) {
        pthread_create(threads + i, NULL, func, (void *) i);
    }
    for (int i = 0; i < THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < THREADS; ++i) {
        printf("%.10g\n", data[i]);
    }
}

