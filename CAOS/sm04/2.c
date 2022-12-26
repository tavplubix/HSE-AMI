#include <stdbool.h>

void
normalize_path(char *buf)
{
    char *norm = buf;
    bool first_slash = true;
    while (*buf) {
        if (*buf == '/' && !first_slash) {
            ++buf;
            continue;
        }
        if (*buf == '/' && first_slash) {
            first_slash = false;
        }
        if (*buf != '/' && !first_slash) {
            first_slash = true;
        }
        *norm++ = *buf++;
    }
    *norm = '\0';
}

