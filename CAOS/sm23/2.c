#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile bool found = false;
volatile uint64_t base;

void *
find_primes(void *arg)
{
    uint64_t num = base;
    if (num % 2 == 0) {
        ++num;
    }
    while(1) {
        bool isprime = true;
        uint64_t sq = sqrt(num) + 1;
        for (uint64_t i = 2; i <= sq; ++i) {
            if (num % i == 0) {
                isprime = false;
                break;
            }
        }
        if (isprime) {
            pthread_mutex_lock(&mutex);
            base = num;
            found = true;
            pthread_cond_signal(&condvar);
            pthread_mutex_unlock(&mutex);
        }
        num += 2;
    }
}

int
main()
{
    uint32_t count;
    scanf("%"SCNu64"%"SCNu32, &base, &count);
    pthread_t thread;
    pthread_create(&thread, NULL, find_primes, NULL);
    pthread_detach(thread);
    while (count--) {
        uint64_t prime;
        pthread_mutex_lock(&mutex);
        if (!found) {
            pthread_cond_wait(&condvar, &mutex);
        }
        found = false;
        prime = base;
        pthread_mutex_unlock(&mutex);
        printf("%"PRIu64"\n", prime);
    }
    exit(0);
}

