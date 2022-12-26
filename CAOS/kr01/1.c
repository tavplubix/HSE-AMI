#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

bool
isodigit(int c)
{
    return '0' <= c && c < '8';
}

bool read_oct_modM(FILE *in, int32_t M, int64_t *res)
{
    enum { BASE = 8 };
    *res = 0;
    bool skip = true;
    int current_char;
    while ((current_char = fgetc(in)) != EOF) {
        if (skip && isodigit(current_char)) {
            skip = false;
        }
        if (!skip && isodigit(current_char)) {
            *res *= BASE;
            *res += current_char - '0';
            *res %= M;
        }
        if (!skip && !isodigit(current_char)) {
            return true;
        }
    }
    return !skip;
}

int
main()
{
    int32_t M;
    scanf("%"SCNd32, &M);
    int64_t min_modM;
    if (!read_oct_modM(stdin, M, &min_modM)) {
        printf("%"PRId32"\n", M);
        return 0;
    }
    int64_t modM;
    while (read_oct_modM(stdin, M, &modM)) {
        if (min_modM > modM) {
            min_modM = modM;
        }
    }
    printf("%"PRId64"\n", min_modM);
}



