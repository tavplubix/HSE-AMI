#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node
{
    double       key;
    struct Node *left;
    struct Node *right;
    char        *str;
} Node;

extern int tree_find(struct Node* root, double key, char** pstr);

void insert(Node** root, double key, const char* str) {
    if (*root == NULL) {
        *root = malloc(sizeof(Node) + strlen(str) + 1);
        (*root)->left = (*root)->right = NULL;
        (*root)->key = key;
        (*root)->str = *root + 1;
        strcpy((*root)->str, str);
        return;
    }
    if (key < (*root)->key) {
        insert(&((*root)->left), key, str);
    } else if (key > (*root)->key) {
        insert(&((*root)->right), key, str);
    }
}

int main() {
    double k = 0;
    char s[64];
    s[0] = 0;
    Node *root = NULL;
    while (1) {
        scanf("%lf%s", &k, s);
        if (k == 42.0)
            break;
        insert(&root, k, s);
    }
    printf("end of input\n");
    char* fstr = s;
    while (1) {
        scanf("%lf", &k);
        if (k == 42.0)
            break;
        int res = tree_find(root, k, &fstr);
        printf("res = %d, str = %s\n", res, fstr);
    }
}


