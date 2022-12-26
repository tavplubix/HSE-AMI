#include <stdio.h>
#include <ctype.h>

int main() {
    for (int a = 0; a < 255; ++a)
        printf("%d\t%c\t%d\n", a, (char)a, islower(a));
}
