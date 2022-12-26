#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


// ============================================================================
//                       DynArr implementation
// ============================================================================

typedef struct DynArr
{
    size_t size;
    size_t capacity;
    size_t elem_size;
    void *data;
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
    fprintf(stderr, "debug: expand_if_possible: bufer resize from %zu to %zu\n", 
                    da->capacity, new_capacity);
    int *new_arr = realloc(da->data, new_capacity * da->elem_size);
    while (new_arr == NULL && new_capacity != da->capacity) {
        multiplier -= 1;
        multiplier /= DIMINUTION_COEFFICIENT;
        multiplier += 1;
        new_capacity = da->capacity * multiplier;
        fprintf(stderr, "debug: expand_if_possible: realloc returned NULL,\
                        another atempt: multiplier = %f, new_capacity = %zu\n",
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
push_back_str(DynArr *da, const void *val)
{
    if (da->size == da->capacity && expand_if_possible(da) == false) {
        return false;
    }
    strncpy(da->data + da->size++ * da->elem_size, val, NAME_MAX);
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


// ============================================================================

typedef int(*cmp_t)(const void *, const void *);

static void
add_name(char *path, size_t len, const char *name)
{
    if (len == 0 || (path[len - 1] != '/' && len != PATH_MAX)) {
        path[len++] = '/';
    }
    if (PATH_MAX <= len) {
        return;
    }
    strncpy(path + len, name, PATH_MAX - len);
}

static DynArr
sorted_subdirs(char *root, DIR *dir)
{
    DynArr subdirs = construct_dynamic_array(NAME_MAX);
    struct dirent *ent;
    size_t root_len = strnlen(root, PATH_MAX);
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        struct stat st;
        add_name(root, root_len, ent->d_name);
        if (lstat(root, &st) == 0 && !S_ISDIR(st.st_mode)) {
            continue;
        }
        if (!push_back_str(&subdirs, ent->d_name)) {
            exit(1);
        }
    }
    qsort(subdirs.data, subdirs.size, subdirs.elem_size, (cmp_t) *strcasecmp);
    return subdirs;
}

static void
dir_dfs(char *root, DIR *dir)
{
    if (dir == NULL) {
        return;
    }
    size_t root_len = strnlen(root, PATH_MAX);
    DynArr subdirs = sorted_subdirs(root, dir);
    closedir(dir);
    for (size_t i = 0; i < subdirs.size; ++i) {
        add_name(root, root_len, at(&subdirs, i));
        DIR *next = opendir(root);
        if (next != NULL) {
            printf("cd %s\n", (const char *) at(&subdirs, i));
            // printf("%s\n", root);
            dir_dfs(root, next);
            puts("cd ..");
        }
    }
    destruct_dynamic_array(&subdirs);
}

int
main(int argc, char **argv)
{
    char buf[PATH_MAX];
    strncpy(buf, argv[1], PATH_MAX);
    DIR *d = opendir(buf);
    dir_dfs(buf, d);
}

