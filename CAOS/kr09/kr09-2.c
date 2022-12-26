#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

_Atomic int to_print = 1;

struct FileInfo
{
    int i, argc;
    char** argv;
};

void* func(void* arg) {
    struct FileInfo* cur = arg;
    FILE *cur_file = fopen(cur -> argv[cur -> i], "r");
    unsigned long long a, b, n;
    fscanf(cur_file, "%llu %llu %llu", &a, &b, &n);
    a = a % n;
    b = b % n;
    unsigned long long res;
    if (2 * n < n) {
        if ((a + b) < a) {
            res = n - a;
            res = b - res;
        } else {
            res = (a + b) % n;
        }
    } else {
        res = (a + b) % n;
    }
    fclose(cur_file);
    while  ((cur -> i) != ((cur -> argc) - to_print) && (to_print <= (cur -> argc))) {
        sched_yield();
    }
    ++to_print;
    printf("%llu\n", res);
    return NULL;
}

    

int main(int argc, char** argv) {
    if (argc == 1) {
        return 0;
    }
    pthread_t ids[argc];
    struct FileInfo* infos[argc];
    for (size_t j = 0; j < argc; ++j) {
        infos[j] = calloc(1, sizeof(struct FileInfo));
    }
    //struct FileInfo* first = calloc(1, sizeof(struct FileInfo));
    //first -> i = 1;
    //first -> argv = argv;
    //first -> argc = argc;
    //pthread_t id;
    //pthread_create(&id, NULL, func, (void*)first);
    //pthread_join(id, NULL);
    for (size_t j = 0; j < argc; ++j) {
        infos[j] -> i = j + 1;
        infos[j] -> argv = argv;
        infos[j] -> argc = argc;
    }
    for (size_t i = 0; i < argc; ++i) {
        pthread_create(&ids[i], NULL, func, (void*)infos[i]);
    }
    for (int i = 0; i < argc; ++i) {
        pthread_join(ids[i], NULL);
    }
}
