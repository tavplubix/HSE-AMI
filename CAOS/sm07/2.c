#include <stdlib.h>
#include <stddef.h>

static int
cmp(const void *a, const void *b, void *data)
{
    int ia = *(int *) a;
    int ib = *(int *) b;
    int *idata = (int *)data;
    if (idata[ia] < idata[ib]) {
        return -1;
    } else if (idata[ia] > idata[ib]) {
        return 1;
    } else if (ia < ib) {
        return -1;
    } else if (ia > ib) {
        return 1;
    } else {
        return 0;
    }
}


void
process(size_t count, const int *data, int *order)
{
    for (size_t i = 0; i < count; ++i) {
        order[i] = i;
    }
    qsort_r(order, count, sizeof(int), cmp, (int *) data);
}



