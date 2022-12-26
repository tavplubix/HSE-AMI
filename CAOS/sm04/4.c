#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


bool fits_into_intx_t(int64_t val, unsigned x) {
    if (x == 0) {
        return false;
    }
    
    // val is in [-2^63, 2^63-1]
    uint64_t uval = val;
    uint64_t mask = ~(uint64_t)0 << (x - 1);
    
    // if val >= 0 then uval == val
    //     if val fits into intx_t 
    //         then val is in [0, 2^(x-1) - 1]
    //         then uval is in [0, 2^(x-1) - 1]
    //         then (uval & mask) == 0
    // if val < 0 then uval == val + 2^64 and uval are in [2^63, 2^64 - 1]
    //     if val fits into intx_t
    //         then val is in [-2^(x-1), -1]
    //         then uval is in [2^64 - 1 - 2^(x-1) + 1, 2^64 - 1]
    //         then (uval & mask) == mask
    return (uval & mask) == 0 || (uval & mask) == mask;
}

bool
try_convert_to_int32_t(const char *str, int32_t *res)
{
    enum
    { 
        BASE = 10,
        BITS_IN_BYTE = 8
    };
    char *first_invalid;
    errno = 0;
    int64_t res64 = strtol(str, &first_invalid, BASE);
    if (errno == ERANGE || str == first_invalid || *first_invalid) {
        return false;
    }
    if (fits_into_intx_t(res64, BITS_IN_BYTE * sizeof(int32_t))) {
        *res = res64;
        return true;
    }
    return false;
}

bool
try_conver_to_double(const char *str, double *res)
{
    char *first_invalid;
    errno = 0;
    *res = strtod(str, &first_invalid);
    if (errno != 0 || str == first_invalid || *first_invalid) {
        return false;
    }
    return true;
}

bool
is_integer(const char *str)
{
    if (*str == '\0') {
        return false;
    }
    if ((*str == '+' || *str == '-') && !isdigit(*++str)) {
        return false;
    }
    while(isdigit(*str++)) {}
    return *str == '\0';
}

bool
is_delimiter(int character)
{
    switch(character) {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
        return true;
    default:
        return false;
    }   
}

bool
try_read_next_token(FILE *f, char *token, size_t max_token_size)
{
    size_t token_size = 0;
    bool skip = true;
    int current_char;
    while ((current_char = fgetc(f)) != EOF) {
        if (!is_delimiter(current_char)) {
            if ((0 <= current_char && current_char < 32) 
                || current_char == 127) {
                return false;   // invalid format: unallowed characters
            }
            if (skip) {
                skip = false;
            }
            if (!skip) {
                if (token_size == max_token_size) {
                    return false;   // invalid format: too long token
                }
                token[token_size++] = current_char;
            }
        }
        if (!skip && is_delimiter(current_char)) {
            token[token_size] = '\0';
            return true;
        }
    }
    token[token_size] = '\0';
    return !skip;   // OK or EOF
}

void
process_file(FILE *f)
{
    enum { MAX_TOKEN_SIZE = 63 };
    int32_t max_int32 = INT32_MIN;
    bool int32_found = false;
    double min_double = HUGE_VAL;
    bool double_found = false;
    char buf[MAX_TOKEN_SIZE + 1];
    while (try_read_next_token(f, buf, MAX_TOKEN_SIZE)) {
        int32_t cur_int;
        double cur_double;
        if (try_convert_to_int32_t(buf, &cur_int)) {
            int32_found = true;
            if (max_int32 < cur_int) {
                max_int32 = cur_int;
            }
        } else if (!is_integer(buf) && try_conver_to_double(buf, &cur_double)) {
            double_found = true;
            if (isnan(cur_double)) {
                min_double = cur_double;
            }
            if (min_double > cur_double) {
                min_double = cur_double;
            }
        }
    }
    if (feof(f)) {
        // try_read_next_token returned false becouse of EOF
        if (!int32_found) {
            fputs("error: no 32-bit signed integer found\n", stderr);
            exit(1);
        }
        if (!double_found) {
            fputs("error: no double found\n", stderr);
            exit(1);
        }
        printf("%"PRId32" %.10g\n", max_int32, min_double);
    } else {
        // try_read_next_token returned false becouse of invalid file format
        fputs("error: invalid file format\n", stderr);
        exit(1);
    }
}

int
main(int argc, char **argv)
{
    if (2 < argc) {
        fputs("error: more than one argument provided\n", stderr);
        exit(1);
    }
    FILE *f = stdin;
    if (argc == 2) {
        errno = 0;
        f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "error: cannot open file: %s\n", strerror(errno));
            exit(1);
        }
    }
    process_file(f);
    fclose(f);
}




