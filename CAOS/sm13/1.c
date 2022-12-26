#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int
main()
{
    time_t now = time(NULL);
    if (now == (time_t) -1) {
        return 1;
    }
    int32_t days;
    while (scanf("%"SCNd32, &days) != EOF) {
        enum { SECONDS_IN_DAY = 24*60*60 };
        time_t now_after_days;
        struct tm *t;
        enum { BUF_SIZE = 128 };
        char buf[BUF_SIZE];
        if (__builtin_mul_overflow(days, SECONDS_IN_DAY, &now_after_days)) {
            fputs("OVERFLOW\n", stdout);
        }
        else if (__builtin_add_overflow(now, now_after_days, &now_after_days)) {
            fputs("OVERFLOW\n", stdout);
        }
        else if ((t = localtime(&now_after_days)) == NULL) {
            fputs("OVERFLOW\n", stdout);
        }
        else if (!strftime(buf, BUF_SIZE, "%Y-%m-%d", t)) {
            fputs("OVERFLOW\n", stdout);
        } else {
            printf("%s\n", buf);
        }
    }
}


