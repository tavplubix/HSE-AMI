#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include "poliz.h"

// ============================================================================
//                       DynArr implementation
// ============================================================================

typedef struct DynArr
{
    size_t size;
    size_t capacity;
    size_t elem_size;
    void *data;         // why there is no templates in C?  :'(
} DynArr;

enum
{ 
    START_ARR_SIZE = 1024,
    START_MULTIPLIER = 2,
    DIMINUTION_COEFFICIENT = 4
};

static DynArr
construct_dynamic_array(size_t elem_size)
{
    DynArr da;
    da.size = 0;
    da.elem_size = elem_size;
    da.data = malloc(START_ARR_SIZE * elem_size);
    if (da.data == NULL) {
        da.capacity = 0;
    } else {
        da.capacity = START_ARR_SIZE;
    }
    return da;
}

static void
destruct_dynamic_array(DynArr *da)
{
    free(da->data);
    da->data = NULL;
    da->size = da->capacity = 0;
}

static bool
expand_if_possible(DynArr *da)
{ 
    if (da == NULL || da->elem_size == 0) {
        return false;
    }
    if (da->capacity == 0) {
        *da = construct_dynamic_array(da->elem_size);
        return da->data != NULL;
    }
    double multiplier = START_MULTIPLIER;
    size_t new_capacity = da->capacity * multiplier;
    fprintf(stderr, "debug: expand_if_possible: bufer resize from %ld to %ld\n", 
                    da->capacity, new_capacity);
    int *new_arr = realloc(da->data, new_capacity * da->elem_size);
    while (new_arr == NULL && new_capacity != da->capacity) {
        multiplier -= 1;
        multiplier /= DIMINUTION_COEFFICIENT;
        multiplier += 1;
        new_capacity = da->capacity * multiplier;
        fprintf(stderr, "debug: expand_if_possible: realloc returned NULL,\
                        another atempt: multiplier = %f, new_capacity = %ld\n",
                        multiplier, new_capacity);
        new_arr = realloc(da->data, new_capacity * da->elem_size);
    }
    if (new_capacity == da->capacity) {
        fprintf(stderr, "error: expand_if_possible: cannot allocate memory\n");
        return false;
    } 
    da->data = new_arr;
    da->capacity = new_capacity;
    return true;
}


static bool
push_back(DynArr *da, const void *val)
{
    if (da->size == da->capacity && expand_if_possible(da) == false) {
        return false;
    }
    memcpy(da->data + da->size++ * da->elem_size, val, da->elem_size);
    return true;
}

static void *
at(DynArr *da, size_t index)
{
    if (da->size <= index) {
        return NULL;
    }
    return da->data + index * da->elem_size;
}

static void *
back(DynArr *da)
{
    if (da->size) {
        return at(da, da->size - 1);
    }
    return NULL;
}

static bool
pop_back(DynArr *da)
{
    if (da->size) {
        --da->size;
        return true;
    }
    return false;
}


// ============================================================================
//                          PolizState implementation
// ============================================================================

struct PolizState
{
    DynArr stack;
    int last_error;
};

struct PolizState *
poliz_new_state()
{
    struct PolizState *ps = malloc(sizeof(struct PolizState));
    if (ps != NULL) {
        ps->stack.size = 0;
        ps->stack.capacity = 0;
        ps->stack.elem_size = sizeof(int32_t);
        ps->stack.data = NULL;
        ps->last_error = PE_OK;
    }
    return ps;
}

void
poliz_free_state(struct PolizState *ps)
{
    if (ps != NULL) {
        destruct_dynamic_array(&ps->stack);
    }
    free(ps);
}

int
poliz_last_error(struct PolizState *ps)
{
    if (ps == NULL) {
        return PE_OUT_OF_MEMORY;
    }
    return ps->last_error;
}


// ============================================================================
//                                  Handlers
// ============================================================================

// I hope nobody will build this code for architectures, 
// where sizeof(int) < sizeof(int32_t), because I have to pass 
// 32bit integers to handlers, but handlers have argument of
// type int. And I cannot change type of the argument.
// So I suggest I can safely cast int32_t to int.

enum { P_INT_BITS = 32 };
static bool fits_into_intx_t(int64_t val, unsigned x);


typedef int64_t (*arithmetic_operation_t)(int64_t a, int64_t b);

int64_t padd(int64_t a, int64_t b)
{
    return a + b;
}
int64_t psub(int64_t a, int64_t b)
{
    return a - b;
}
int64_t pmul(int64_t a, int64_t b)
{
    return a * b;
}
int64_t pmod(int64_t a, int64_t b)
{
    if (b < 0) {
        b = -b;
    }
    return ((a % b) + b) % b;
}
int64_t pdiv(int64_t a, int64_t b)
{
    if (a < 0) {
        return (a - pmod(a, b)) / b;
    }
    return a / b;
}


static int
safe_arithmetic_operation(struct PolizState *state, arithmetic_operation_t op)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    if (state->stack.size < 2) {
        return -(state->last_error = PE_STACK_UNDERFLOW);
    }
    int32_t *s0 = back(&state->stack);
    pop_back(&state->stack);
    int32_t *s1 = back(&state->stack);
    pop_back(&state->stack);
    if (s0 == NULL || s1 == NULL) {
        return -(state->last_error = PE_STACK_UNDERFLOW);
    }
    if (*s0 == 0 && (op == pdiv || op == pmod)) {
        return -(state->last_error = PE_DIVISION_BY_ZERO);
    }
    int64_t res64 = op(*s1, *s0);
    if (!fits_into_intx_t(res64, P_INT_BITS)) {
        return -(state->last_error = PE_INT_OVERFLOW);
    }
    int32_t res = res64;
    if (!push_back(&state->stack, &res)) {
        return -(state->last_error = PE_OUT_OF_MEMORY);
    }
    return 0;
 
}

static int num_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return state->last_error;
    }
    int32_t new_elem = iextra;
    if (!push_back(&state->stack, &new_elem)) {
        return -(state->last_error = PE_OUT_OF_MEMORY);
    }
    return 0;
}

static int add_handler(struct PolizState *state, int iextra)
{
    return safe_arithmetic_operation(state, padd);
}

static int sub_handler(struct PolizState *state, int iextra)
{
    return safe_arithmetic_operation(state, psub);
}

static int mul_handler(struct PolizState *state, int iextra)
{
    return safe_arithmetic_operation(state, pmul);
}

static int div_handler(struct PolizState *state, int iextra)
{
    return safe_arithmetic_operation(state, pdiv);
}

static int mod_handler(struct PolizState *state, int iextra)
{
    return safe_arithmetic_operation(state, pmod);
}

static int neg_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    int32_t *s0;
    if ((s0 = back(&state->stack)) == NULL) {
        return -(state->last_error = PE_STACK_UNDERFLOW);
    }
    if (*s0 == INT32_MIN) {
        return -(state->last_error = PE_INT_OVERFLOW);
    }
    *s0 = -*s0;
    return 0;
}

static int read_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }

    enum { BUF_SIZE = 32 };
    char buf[BUF_SIZE];
    bool spaces = true;
    int cur;
    size_t len = 0;
    while ((cur = fgetc(stdin)) != EOF) {
        if (spaces && !isspace(cur)) {
            spaces = false;
        }
        if (!spaces && !isspace(cur) && len + 1 < BUF_SIZE) {
            buf[len++] = cur;
        }
        if (!spaces && isspace(cur)) {
            break;
        }
    }
    buf[len++] = '\0';

    errno = 0;
    char *end;
    int64_t new_elem64 = strtoll(buf, &end, 10);
    if (len == BUF_SIZE || errno || !*buf || *end
        || !fits_into_intx_t(new_elem64, P_INT_BITS)) {
        return -(state->last_error = PE_READ_FAILED);
    }
    int32_t new_elem = new_elem64;
    if (!push_back(&state->stack, &new_elem)) {
        return -(state->last_error = PE_OUT_OF_MEMORY);
    }
    return 0;
}

static int write_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    int32_t *s0;
    if ((s0 = back(&state->stack)) == NULL) {
        return -(state->last_error = PE_STACK_UNDERFLOW);
    }
    if (printf("%"PRId32, *s0) < 0) {
        // suggests printf never fails
    }
    pop_back(&state->stack);
    return 0;
}

static int newline_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    if (fputc('\n', stdout) == EOF) {
        // suggests fputc never fails
    }
    return 0; 
}

static int del_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    if (!pop_back(&state->stack)) {
        return -(state->last_error = PE_STACK_UNDERFLOW);
    }
    return 0;
}

static int copy_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    if (iextra < 0 || state->stack.size <= iextra) {
        return -(state->last_error = PE_INVALID_INDEX);
    }
    int32_t snum = *(int32_t *) at(&state->stack, 
                                    state->stack.size - iextra - 1);
    if (!push_back(&state->stack, &snum)) {
        return -(state->last_error = PE_OUT_OF_MEMORY);
    }
    return 0;
}

static int swap_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    if (iextra == 0) {
        return 0;
    }
    if (iextra < 0 || state->stack.size <= iextra) {
        return -(state->last_error = PE_INVALID_INDEX);
    }
    int32_t *s0 = back(&state->stack);
    int32_t *snum = at(&state->stack, state->stack.size - iextra - 1);
    int32_t tmp = *s0;
    *s0 = *snum;
    *snum = tmp;
    return 0;
}

static int error_handler(struct PolizState *state, int iextra)
{
    if (state->last_error != PE_OK) {
        return -state->last_error;
    }
    return -(state->last_error = iextra);
}


// ============================================================================
//                          Compilation and parsing
// ============================================================================


static const char *
find_token(const char *str)
{
    while (isspace(*str)) {
        ++str;
    }
    return str;
}

static bool
fits_into_intx_t(int64_t val, unsigned x) {
    if (x == 0) {
        return false;
    }
    
    // val is in [-2^63, 2^63-1]
    uint64_t uval = val;
    uint64_t mask = ~(uint64_t)0 << (x - 1);
    
    // if val >= 0 then uval == val
    //     if val fits into intx_t 
    //         then val is in [0, 2^(x-1) - 1]
    //         then uval is in [0, 2^(x-1) - 1]
    //         then (uval & mask) == 0
    // if val < 0 then uval == val + 2^64 and uval are in [2^63, 2^64 - 1]
    //     if val fits into intx_t
    //         then val is in [-2^(x-1), -1]
    //         then uval is in [2^64 - 1 - 2^(x-1) + 1, 2^64 - 1]
    //         then (uval & mask) == mask
    return (uval & mask) == 0 || (uval & mask) == mask;
}

static bool
is_space_or_null(char c)
{
    return isspace(c) || c == '\0';
}

struct PolizItem *
poliz_compile(const char *str)
{
    DynArr pi_arr = construct_dynamic_array(sizeof(struct PolizItem));
    if (pi_arr.data == NULL) {
        return NULL;
    }

    const char *cmd = find_token(str);
    int error = PE_OK;
    while (*cmd) {
        struct PolizItem next_item = { NULL, 0 };
        switch (*cmd) {
        case '+':
            if (!is_space_or_null(*(cmd + 1))) {   
                goto parse_num;           // it's not "+", it's "+NUM"
            }
            next_item.handler = add_handler;
            break;
        case '-':
            if (!is_space_or_null(*(cmd + 1))) {
                goto parse_num;           // it's not "-", it's "-NUM"
            }
            next_item.handler = sub_handler;
            break;
        case '*':
            next_item.handler = mul_handler;
            break;
        case '/':
            next_item.handler = div_handler;
            break;
        case '%':
            next_item.handler = mod_handler;
            break;
        case '#':
            next_item.handler = neg_handler;
            break;
        case 'r':
            next_item.handler = read_handler;
            break;
        case 'w':
            next_item.handler = write_handler;
            break;
        case 'n':
            next_item.handler = newline_handler;
            break;
        case ';':
            next_item.handler = del_handler;
            break;

        // Next cases suggest *cmd is valid:
        //
        //   -  *cmd begins with "NUM_" or "dNUM_" or "sNUM_" or "d_" or "s_",
        //      where NUM is a decimal number (not random string)
        //      and _ is a space character or '\0'
        //
        //   -  if *cmd begins with "dNUM_" or "sNUM_"
        //      then NUM >= 0

        case 'd':
            next_item.handler = copy_handler;
            ++cmd;
            goto parse_num;
        case 's':
            next_item.handler = swap_handler;
            ++cmd;
            goto parse_num;
        default:
        parse_num:
            if (next_item.handler == NULL) {
                next_item.handler = num_handler;
            }
            char *end;
            errno = 0;
            // if *cmd is "d_" it will be interpreted like "d0_"
            long long num = strtoll(cmd, &end, 10);
            if (next_item.handler == swap_handler
                && is_space_or_null(*cmd)) {
                // if *cmd is "s_" interpret it like "s1_"
                num = 1;
            }
            if (!errno && fits_into_intx_t(num, sizeof(int32_t) * CHAR_BIT)) {
                next_item.iextra = num; 
            } else {
                error = PE_INT_OVERFLOW;
            }
            cmd = end - 1;
        }
        
        if (!push_back(&pi_arr, &next_item)) {
            fputs("error: poliz_colmpile: not enough memory for next item\n",
                    stderr);
            exit(1);
        }
        if (error != PE_OK) {
            struct PolizItem *err_item = back(&pi_arr); 
            err_item->handler = error_handler;
            err_item->iextra = error;
            break;
        }
        cmd = find_token(cmd + 1);
    }
    
    
    return pi_arr.data;
}


