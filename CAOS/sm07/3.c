#include <stddef.h>

struct BSearchResult
{
    size_t low;
    size_t high;
    int result;
};

struct BSearchResult
bsearch2(
        const void *key,
        const void *base,
        size_t nmemb,
        size_t size,
        int (*compar)(const void *p1, const void *p2, void *user),
        void *user)
{
    const void *beg = base;
    const void *end = base + nmemb * size;
    while (end != beg) {
        const void *mid = beg + (end - beg) / size / 2 * size;
        if ((compar)(mid, key, user) < 0) {
            beg = mid + 1 * size;
        } else {
            end = mid;
        }
    }

    struct BSearchResult result;
    if (nmemb == 0 || (compar)(beg, key, user)) {
        result.result = 0;
        result.low = result.high = (end - base) / size;
    } else {
        result.result = 1;
        result.low = (beg - base) / size;
        beg = base;
        end = base + nmemb * size;
        while(beg != end) {
            const void *mid = beg + (end - beg) / size / 2 * size;
            if ((compar)(mid, key, user) > 0) {
                end = mid;
            } else {
                beg = mid + 1 * size;
            }
        }
        result.high = (beg - base) / size;
    }
    return result;
}


