#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* func(void* arg) {
    long long sum = 0;
    int val;
    while(scanf("%i", &val) == 1) {
        sum += val;
        sched_yield(); //передать контроль следующей нити
    }
    *(long long*)arg = sum;
    return NULL;
}

int main(int argc, char** argv) {
    int count = strtol(argv[1], NULL, 10);
    pthread_t* ids = calloc(count, sizeof(ids[0]));
    long long* result = calloc(count, sizeof(long long));
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setstacksize(&tattr, sysconf(_SC_THREAD_STACK_MIN));
    pthread_attr_setguardsize(&tattr, 0);
    for (int i = 0; i < count; ++i) {
        pthread_create(&ids[i], &tattr, func, &result[i]);
    }
    pthread_attr_destroy(&tattr); 
    for (int i = 0; i < count; ++i) {
        pthread_join(ids[i], NULL);
    }
    long long total = 0;
    for (int i = 0; i < count; ++i) {
        total += result[i];
    }
    printf("%lld\n", total);
}
