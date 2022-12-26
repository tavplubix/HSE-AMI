#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* func(void* arg) {
    int val;
    if (scanf("%i", &val) != 1) {
        return NULL;
    }
    pthread_t id;
    pthread_create(&id, NULL, func, NULL);
    pthread_join(id, NULL);
    printf("%i\n", val);
    return NULL;
}

int main() {
    pthread_t id;
    pthread_create(&id, NULL, func, NULL);
    pthread_join(id, NULL);
}
