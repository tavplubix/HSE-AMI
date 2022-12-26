#include <stdio.h>
#include <ctype.h>

int
main(void)
{
    long long sum = 0;
    int next_char;
    while ((next_char = getchar_unlocked()) != EOF)
    {
        if (isdigit(next_char))
        {
            sum += next_char - '0';
        }
    }
    printf("%lld\n", sum);
}
