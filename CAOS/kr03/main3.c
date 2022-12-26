#include <stdio.h>
#include <string.h>

extern size_t mystrspn(const char *s, const char *accept);

int main() {
    char s1[64];
    char s2[64];
    s1[0] = s2[0] = 0;
    scanf("%s%s", s1, s2);
    printf("%d\n%d\n", mystrspn(s1, s2), strspn(s1, s2));

}


