#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


void
to_balanced_ternary_system(int32_t n, char *res)
{
    if (n == 0) {
        *res++ = '0';
        *res = '\0';
        return;
    }
    char *res_begin = res;
    uint64_t abs_n = n;
    if (n < 0) {
        abs_n = -(int64_t) n;
    }

    while (abs_n > 2) {
        int rem = abs_n % 3;
        abs_n /= 3;
        if (rem == 2) {
            *res++ = 'a';
            ++abs_n;
        } else {
            *res++ = '0' + rem;
        }
    }
    if (abs_n == 1) {
        *res++ = '1';
    } else if (abs_n == 2) {
        *res++ = 'a';
        *res++ = '1';
    }
    *res = '\0';

    if (n < 0) {
        res = res_begin;
        while (*res) {
            if (*res == '1') {
                *res = 'a';
            } else if (*res == 'a') {
                *res = '1';
            }
            ++res;
        }
    }
    --res;
    while (res_begin < res) {
        char tmp = *res;
        *res-- = *res_begin;
        *res_begin++ = tmp;
    }
}


int
main()
{
    int32_t decimal;
    enum { BUF_SIZE = 64 };
    char buf[BUF_SIZE];
    bool empty_input = true;
    while (scanf("%"SCNd32, &decimal) != EOF) {
        empty_input = false;
        to_balanced_ternary_system(decimal, buf);
        printf("%s\n", buf);
    }
    if (empty_input) {
        putchar('\n');
    }
}

