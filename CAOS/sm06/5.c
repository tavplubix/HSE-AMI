#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

size_t
utf8_char_size(unsigned char first_byte)
{
    unsigned char mask = 0x80;
    if ((mask & first_byte) == 0) {   //first_byte = 0b0xxxxxxx
        return 1;
    }
    mask >>= 2;         //       mask = 0b00100000
    size_t size = 2;    // first_byte = 0b110xxxxx
    while (mask & first_byte) { 
        ++size;         //       size = 3,            4,            ...
        mask >>= 1;     //       mask = 0b00010000,   0b00001000,   ...
    }                   // first_bite = 0b1110xxxx,   0b11110xxx,   ...
    return size;
}

typedef struct String
{
    char *c_str;
    size_t size;
    size_t capacity;
} String;

String
construct_string()
{
    enum { MIN_SIZE = 64 };
    String s;
    s.capacity = MIN_SIZE;
    s.c_str = malloc(s.capacity + 1);
    if (s.c_str == NULL) {
        exit(1);
    }
    s.size = 0;
    *s.c_str = '\0';
    return s;
}

void
string_append_char(String *s, char c)
{
    enum { MULTIPLIER = 2 };
    if (s->size == s->capacity) {
        s->capacity *= MULTIPLIER;
        s->c_str = realloc(s->c_str, s->capacity + 1);
        if (s->c_str == NULL) {
            exit(1);
        }
    }
    s->c_str[s->size++] = c;
    s->c_str[s->size] = '\0';
}

void
erase_string(String *s)
{
    s->size = 0;
    *s->c_str = '\0';
}

void
swap_strings(String *s1, String *s2)
{
    String tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}


void
destruct_string(String *s)
{
    free(s->c_str);
    s->c_str = NULL;
    s->size = s->capacity = 0;
}

bool
is_space(int c)
{
    enum { MAX_SPACE_CODE = 0x20 };
    return c <= MAX_SPACE_CODE || c == EOF;
}


int
main()
{
    int cur_first_byte;
    size_t max_len = 0;
    size_t cur_len = 0;
    bool spaces = false;
    String longest_word = construct_string();
    String cur_word = construct_string();
    do {
        cur_first_byte = fgetc(stdin);
        if (spaces && !is_space(cur_first_byte)) {
            // stop skipping spaces
            spaces = false;
        }
        if (!spaces && !is_space(cur_first_byte)) {
            // read next utf8-char of this word
            string_append_char(&cur_word, cur_first_byte);
            size_t char_size = utf8_char_size(cur_first_byte);
            for (size_t i = 1; i < char_size; ++i) {
                string_append_char(&cur_word, fgetc(stdin));
            }
            ++cur_len;
        } else if (is_space(cur_first_byte)) {
            // end of this word (may be EOF)
            if (max_len < cur_len) {
                max_len = cur_len;
                swap_strings(&cur_word, &longest_word);
            }
            cur_len = 0;
            erase_string(&cur_word);
        }
    } while (cur_first_byte != EOF);
    destruct_string(&cur_word);
    printf("%zu\n", max_len);
    if (max_len) {
        fputs(longest_word.c_str, stdout);
        fputc('\n', stdout);
    }
    destruct_string(&longest_word);
}


