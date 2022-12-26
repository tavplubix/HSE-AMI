#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
enum { N = 10 };
pthread_t ids[N];

void* func(void* arg) {
    int num = (int)(intptr_t)arg;
    if (num > 0) {
        pthread_join(ids[num - 1], NULL);
    }
    printf("%i\n", num);
    return NULL;
}

int main() {
    for (size_t i = 0; i < N; ++i) {
        pthread_create(&ids[i], NULL, func, (void*)(intptr_t)i);
    }
    pthread_join(ids[N - 1], NULL);
}
