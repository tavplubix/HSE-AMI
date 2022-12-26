#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

    /* because it is not obvious enough */
enum
{
    NUMBER_OF_DECIMAL_DIGITS = 10,
    NUMBER_OF_UPPER_LATIN_LETTERS = 26,
    NUMBER_OF_LOWER_LATIN_LETTERS = 26,
    NUMBER_OF_ELEMENTS = 10 + 26 + 26
};

uint64_t
char_to_flag(int c)
{
    if (c == '#') {
        return 0;
    }
    int shift;
    if (isdigit(c)) {
        shift = c - '0';
    } else if (isupper(c)) {
        shift = NUMBER_OF_DECIMAL_DIGITS + c - 'A';
    } else /*if (islower(c))*/ {
        shift =   NUMBER_OF_DECIMAL_DIGITS 
                + NUMBER_OF_UPPER_LATIN_LETTERS + c - 'a';
    }
    return (uint64_t) 1 << shift;
}

void
subset_to_str(
    uint64_t flagset, 
    uint64_t first_bit, 
    char from, 
    char to, 
    char **str)
{
    for (char c = from; c <= to; ++c) {
        if (flagset & first_bit) {
            *(*str)++ = c;
        }
        first_bit <<= 1;
    }
}

void
set_to_str(uint64_t flagset, char *str)
{
    enum { NUMBER_OF_BITS_IN_UINT64_T = 64 };
    uint64_t mask = ~(uint64_t)0;
    mask >>= NUMBER_OF_BITS_IN_UINT64_T - NUMBER_OF_ELEMENTS;
    if ((flagset & mask) == 0) {
        *str++ = '#';
    } else {
        subset_to_str(flagset, 
            (uint64_t) 1, '0', '9', &str);
        subset_to_str(flagset, 
            (uint64_t) 1 << NUMBER_OF_DECIMAL_DIGITS, 'A', 'Z', &str);
        subset_to_str(flagset, 
            (uint64_t) 1 << (NUMBER_OF_DECIMAL_DIGITS
            + NUMBER_OF_UPPER_LATIN_LETTERS), 'a', 'z', &str);
    }
    *str = '\0';
}

enum { MAX_STACK_SIZE = 1024 };
struct Stack
{
    uint64_t stack[MAX_STACK_SIZE];
    size_t size;
};

bool
push(struct Stack *stack, uint64_t elem)
{
    if (stack->size == MAX_STACK_SIZE) {
        return false;
    }
    stack->stack[(stack->size)++] = elem;
    return true;
}

uint64_t
pop(struct Stack *stack)
{
    if (stack->size) {
        return stack->stack[--(stack->size)];
    }
    return 0;
}

enum { BUF_SIZE = 64 };

int 
main()
{
    struct Stack stack;
    stack.size = 0;
    bool after_space = true;
    int current_char;
    while ((current_char = fgetc(stdin)) != EOF) {
        switch(current_char) {
        case '&':
            push(&stack, pop(&stack) & pop(&stack));
            break;
        case '|':
            push(&stack, pop(&stack) | pop(&stack));
            break;
        case '^':
            push(&stack, pop(&stack) ^ pop(&stack));
            break;
        case '~':
            push(&stack, ~pop(&stack));
            break;
        default:
            if (isspace(current_char)) {
                after_space = true;
            } else /*if (isalnum(current_char))*/ {
                if (after_space) {
                    after_space = false;
                    push(&stack, char_to_flag(current_char));
                } else {
                    push(&stack, pop(&stack) | char_to_flag(current_char));
                }
            }
        }
    }
    char result[BUF_SIZE];
    set_to_str(pop(&stack), result);
    printf("%s\n", result);
}

