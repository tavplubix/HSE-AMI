#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum
{ 
    START_BUF_SIZE = 1024,
    START_MULTIPLIER = 2,
    DIMINUTION_COEFFICIENT = 3
};

char *getline2(FILE *f)
{
    double multiplier = START_MULTIPLIER;
    int cur = fgetc(f);
    if (cur == EOF) {
        return NULL;
    }
    size_t buf_capacity = START_BUF_SIZE;
    size_t buf_size = 0;
    char *buf = malloc(buf_capacity);
    if (buf == NULL) {
        return NULL;
    }
    while (cur != EOF) {
        buf[buf_size++] = cur;

        if (buf_size == buf_capacity) {
            size_t new_capacity = buf_capacity * multiplier;
            fprintf(stderr, "debug: getline2: bufer resize from %ld to %ld\n", 
                    buf_capacity, new_capacity);
            char *new_buf = realloc(buf, new_capacity);
            while (new_buf == NULL && new_capacity != buf_capacity) {
                multiplier -= 1;
                multiplier /= DIMINUTION_COEFFICIENT;
                multiplier += 1;
                new_capacity = buf_capacity * multiplier;
                fprintf(stderr, "debug: getline2: realloc returned NULL,\
                        another atempt: multiplier = %f, new_capacity = %ld\n",
                        multiplier, new_capacity);
                new_buf = realloc(buf, new_capacity);
            }
            if (new_capacity == buf_capacity) {
                fprintf(stderr, "error: getline2: cannot allocate memory\n");
                free(buf);
                return NULL;
            }
            buf = new_buf;
            buf_capacity = new_capacity;
        }

        if (cur == '\n') {
            break;
        }
        cur = fgetc(f);
    }
    buf[buf_size] = '\0';
    return buf;
}


