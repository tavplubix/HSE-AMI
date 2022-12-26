#include "5.c"


int main() {
    char *str = NULL;
    size_t str_len = 0;
    getline(&str, &str_len, stdin);
    struct PolizItem *items = poliz_compile(str);
    struct PolizState *state = poliz_new_state();
    for (int i = 0; items[i].handler != NULL; ) {
        int err = items[i].handler(state, items[i].iextra);
        if (err < 0) {
            fprintf(stderr, "error: %d\n", -err);
            break;
        } else if (err > 0) {
            i = err;
        } else {
            ++i;
        }
    }
    poliz_free_state(state);
    free(items);
}



