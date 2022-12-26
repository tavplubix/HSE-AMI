#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    enum { YEAR_BIAS = 1900, MON_BIAS = 1 };
    struct tm t;
    t.tm_sec = t.tm_min = t.tm_hour = 0;
    t.tm_isdst = -1;
    int year, mon, wday;
    scanf("%d%d%d", &year, &mon, &wday);
    t.tm_year = year - YEAR_BIAS;
    t.tm_mon = mon - MON_BIAS;
    for (int i = 1; i <= 31; ++i) {
        t.tm_mday = i;
        mktime(&t);
        if (t.tm_mon != mon - MON_BIAS) {
            break;
        }
        if (t.tm_wday == wday) {
            printf("%d\n", t.tm_mday);
        }
    }
}

