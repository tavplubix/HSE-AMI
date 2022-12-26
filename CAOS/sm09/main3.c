#include <stdio.h>
#include <string.h>

extern int mystrcmp(const char *s1, const char *s2);

int main() {
    char s1[128];
    char s2[128];
    scanf("%s%s", s1, s2);
    printf("%d %d", strcmp(s1, s2), mystrcmp(s1, s2));

}



