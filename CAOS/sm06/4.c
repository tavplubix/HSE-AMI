#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>


int
main()
{
    setlocale(LC_ALL, "");
    int digits = 0;
    int upper = 0;
    int lower = 0;
    wint_t cur_c;
    while ((cur_c = getwc(stdin)) != WEOF) {
        if (iswdigit(cur_c)) {
            ++digits;
        } else if (iswupper(cur_c)) {
            ++upper;
        } else if (iswlower(cur_c)) {
            ++lower;
        }
    }
    printf("%d\n%d\n%d\n", digits, upper, lower);
}


