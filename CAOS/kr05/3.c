#include <string.h>

int
parse_rwx_permissions(const char *str)
{
    if (str == NULL) {
        return -1;
    }
    const char *pattern = "rwxrwxrwx";
    size_t len = strlen(pattern);
    int perms = 0;
    for (size_t i = 0; i < len; ++i) {
        perms <<= 1;
        if (str[i] == pattern[i]) {
            perms |= 1;
        } else if (str[i] != '-') {
            return -1;
        }
    }
    if (str[len] != '\0') {
        return -1;
    }
    return perms;
}

