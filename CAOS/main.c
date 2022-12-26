#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

enum { BASE = 7 };

enum State
{
    BEFORE_DOT,
    AFTER_DOT,
    SPACE
};

int main() 
{
    int current_char = 0;

    double whole = 0;
    double fraction = 0;

    enum State current_state = SPACE;

    while ((current_char = getchar_unlocked()) != EOF) {
        if (isspace(current_char) && current_state != SPACE) {
            current_state = SPACE;
            printf("%.10g\n", fraction + whole);
            whole = 0;
            fraction = 0;
            continue;
        }

        if (current_char == '.' && current_state == BEFORE_DOT) {
            current_state = AFTER_DOT;
            continue;
        }

        if (isdigit(current_char) && current_state == SPACE) {
            current_state = BEFORE_DOT;
        }
        
        if (isdigit(current_char) && current_state == BEFORE_DOT) {
            whole *= BASE;
            whole += current_char - '0';
        }

        if (isdigit(current_char) && current_state == AFTER_DOT) {
            fraction += current_char - '0';
            fraction /= BASE;
        }
    }
    if (current_state != SPACE) {
        printf("%.10g\n", whole + fraction);
    }
    return 0;
}
