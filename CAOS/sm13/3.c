#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>


bool
read_timestamp(FILE* fd, time_t *time)
{
    enum { DATE_SIZE = 11, TIME_SIZE = 9};
    struct tm t;
    if (fscanf(fd, "%d%*c%d%*c%d", &t.tm_year, &t.tm_mon, &t.tm_mday) != 3) {
        return false;
    }
    if (fscanf(fd, "%d%*c%d%*c%d", &t.tm_hour, &t.tm_min, &t.tm_sec) != 3) {
        return false;
    }
    t.tm_year -= 1900;
    t.tm_mon -= 1;
    t.tm_isdst = -1;
    errno = 0;
    *time = mktime(&t);
    return errno == 0;
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        return 1;
    }
    FILE* fd = fopen(argv[1], "r");
    if (fd == NULL) {
        return 1;
    }
    time_t first, second;
    read_timestamp(fd, &first);
    while (read_timestamp(fd, &second)) {
        printf("%lld\n", (long long) second - first);
        first = second;
    }
}

