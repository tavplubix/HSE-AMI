#include <ctype.h>
#include <math.h>
#include <stdio.h>

enum Mode 
{
    SPACES,
    BEFORE_POINT,
    AFTER_POINT
};

enum { RADIX = 7 };

int
main()
{
    int next_char;
    enum Mode current_mode = SPACES;
    double number = 0;
    int digits_after_point = 0;
    while ((next_char = getchar()) != EOF) {
        if (current_mode == SPACES && isdigit(next_char)) {
            current_mode = BEFORE_POINT;
        }
        if (current_mode == BEFORE_POINT && next_char == '.') {
            current_mode = AFTER_POINT;
            continue;
        }
        if (current_mode != SPACES && isspace(next_char)) {
            current_mode = SPACES;
            printf("%.10g\n",
                    number / pow((double) RADIX, digits_after_point));
            number = 0;
            digits_after_point = 0;
        }

        if (current_mode != SPACES) {
            number *= RADIX;
            number += next_char - '0';
            if (current_mode == AFTER_POINT) {
                ++digits_after_point;
            }
        }
    }
    if (current_mode != SPACES) {
        printf("%.10g\n", number / pow((double) RADIX, digits_after_point));
    }
}

