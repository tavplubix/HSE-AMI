#include <ctype.h>

void
skip_spaces(const unsigned char **str) {
    while (**str && isspace(**str)) {
        ++*str;
    }
}

int
mystrspccmp(const char *str1, const char *str2)
{
    const unsigned char *ustr1 = (const unsigned char *) str1;
    const unsigned char *ustr2 = (const unsigned char *) str2;
    while (*ustr1 && *ustr2) {
        skip_spaces(&ustr1);
        skip_spaces(&ustr2);
        if (*ustr1 != *ustr2) {
            return *ustr1 - *ustr2;
        }
        if (*ustr1 && *ustr2) {
            ++ustr1;
            ++ustr2;
        }
    }
    skip_spaces(&ustr1);
    skip_spaces(&ustr2);
    return *ustr1 - *ustr2;
}

