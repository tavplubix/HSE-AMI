#include <stdio.h>


#include "4.c"


int main() {
    RandomGenerator *rr = random_create(0);
    for (int j = 0; j < 100; ++j) {
    printf("%d\n", rr->ops->next(rr));
    }
    rr->ops->destroy(rr);
}



