#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

int
main(int argc, char **argv)
{
    double dsum = 0;
    int number_of_doubles = 0;
    int64_t isum = 0;
    size_t strlen_sum = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == 'd') {
            dsum += strtod(argv[i] + 1, NULL);
            ++number_of_doubles;
        } else if (argv[i][0] == 'i') {
            enum { BASE = 10 };
            isum += strtoll(argv[i] + 1, NULL, BASE);
        } else {
            strlen_sum += strlen(argv[i]);
        }
    }
    if (number_of_doubles) {
        dsum /= number_of_doubles;
    }
    printf("%.10g\n%"PRId64"\n%zu\n", dsum, isum, strlen_sum);
}


