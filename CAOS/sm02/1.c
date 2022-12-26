#include <stdio.h>

int
main(void)
{
    int first_max = 0, second_max = 0;
    int current_number;
    while (scanf("%d", &current_number) != EOF) {
        if (first_max <= current_number) {
            second_max = first_max;
            first_max = current_number;
        } else if (second_max < current_number) {
            second_max = current_number;
        }
    }
    printf("%d\n", first_max * second_max);
}

