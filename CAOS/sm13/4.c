#include <string.h>
#include <fcntl.h>

const char *
perms_to_str(char *buf, size_t size, int perms)
{
    if (size == 0) {
        return buf;
    }
    enum { MAX_SIZE = 10 };
    char str[MAX_SIZE];
    char *pstr = str;
    strcpy(str, "---------");
     
    if (perms & S_IRUSR) {
        *pstr = 'r';
    }
    ++pstr;
    if (perms & S_IWUSR) {
        *pstr = 'w';
    }
    ++pstr;
    if (perms & S_IXUSR) {
        *pstr = 'x';
    }
    ++pstr;
    if (perms & S_IRGRP) {
        *pstr = 'r';
    }
    ++pstr;
    if (perms & S_IWGRP) {
        *pstr = 'w';
    }
    ++pstr;
    if (perms & S_IXGRP) {
        *pstr = 'x';
    }
    ++pstr;
    if (perms & S_IROTH) {
        *pstr = 'r';
    }
    ++pstr;
    if (perms & S_IWOTH) {
        *pstr = 'w';
    }
    ++pstr;
    if (perms & S_IXOTH) {
        *pstr = 'x';
    }
    ++pstr;
    *pstr = '\0';

    enum { SUID_POS = 2, SGID_POS = 5, SVTX_POS = 8  };
    if (perms & S_ISUID && (perms & S_IXGRP || perms & S_IXOTH)) {
        str[SUID_POS] = 's';
    }
    if (perms & S_ISGID && perms & S_IXOTH) {
        str[SGID_POS] = 's';
    }
    if (perms & S_ISVTX && perms & S_IWOTH && perms & S_IXOTH) {
        str[SVTX_POS] = 't';
    }
    
    if (size < MAX_SIZE) {
        str[size - 1] = '\0';
    }
    return strncpy(buf, str, size);
}

