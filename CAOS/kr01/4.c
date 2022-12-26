#include <stdbool.h>
#include <stdint.h>


int64_t
addmod(int64_t a, int64_t b, int64_t c)
{
    bool a_changed = false;
    bool b_changed = false;
    uint64_t ua = a;
    if (a < 0) {
        ++a;
        ua = -a;
        a_changed = true;
    }
    uint64_t ub = b;
    if (b < 0) {
        ++b;
        ub = -b;
        b_changed = true;
    }
    uint64_t uc = c;
    ua %= uc;
    ub %= uc;
    if (a_changed) {
        ++ua;
        ua %= uc;
        ua = uc - ua;
    }
    if (b_changed) {
        ++ub;
        ub %= uc;
        ub = uc - ub;
    }
    uint64_t usum = (ua + ub) % uc;
    return usum;
}

