#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

enum { BYTES_PER_LINE = 4 };

bool
read_next_number(FILE * in, uint32_t * res) {
    *res = 0;
    int hex_digits = 0;
    int current_char;
    while(hex_digits != 2 * BYTES_PER_LINE && (current_char = fgetc(in)) != EOF) {
        if (!isspace(current_char)) {
            *res *= 16;
            if (isalpha(current_char)) {
                *res += tolower(current_char) - 'a' + 10;
            } else {
                *res += current_char - '0';
            }
            ++hex_digits;
        }
    }
    return hex_digits == 2 * BYTES_PER_LINE;
}

bool
skip_shift(FILE * in) {
    bool skip_spaces = true;
    int current_char;
    while((current_char = fgetc(in)) != EOF) {
        if (isspace(current_char) && !skip_spaces) {
            return true;
        }
        skip_spaces = isspace(current_char);
    }
    return false;
}

int
main() {
    int counter = 0;
    bool ok = true;
    while (ok) {
        if (counter % (1 + BYTES_PER_LINE) == 0) {
            ok = skip_shift(stdin);
        } else {
            uint32_t number;
            ok = read_next_number(stdin, &number);
            if (ok) {
                printf("%"PRIu32"\n", number);
            }
        }
        ++counter;
    }
}


