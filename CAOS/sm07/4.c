#include <stdint.h>
#include <stdlib.h>


typedef struct RandomOperations RandomOperations;

typedef struct RandomGenerator
{
    const RandomOperations *ops;
    uint64_t state;
} RandomGenerator;


struct RandomOperations
{
    void (*destroy)(RandomGenerator *);
    int (*next)(RandomGenerator *);
};


void
destroy(RandomGenerator *rr)
{
    free(rr);
}

int
next(RandomGenerator *rr)
{
    const uint64_t multiplier = 1103515245;
    const uint64_t increment = 12345;
    const uint64_t modulus = 0x80000000ull;
    return rr->state = (multiplier * rr->state + increment) % modulus;
}

const RandomOperations default_random_operations = { destroy, next };

RandomGenerator *
random_create(int seed)
{
    RandomGenerator *rr = malloc(sizeof(RandomGenerator));
    rr->ops = &default_random_operations;
    rr->state = seed;
    return rr;
}



