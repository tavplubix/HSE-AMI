#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct TreeNode
{
    struct TreeNode *left;
    struct TreeNode *right;
    int val;
} TreeNode;

typedef struct Tree
{
    TreeNode *root;
} Tree;

Tree
construct_tree()
{
    Tree t;
    t.root = NULL;
    return t;
}

TreeNode **
find(Tree *tree, int val)
{
    TreeNode **node_ptr = &tree->root;
    while (*node_ptr != NULL && (*node_ptr)->val != val) {
        if (val < (*node_ptr)->val) {
            node_ptr = &(*node_ptr)->left;
        } else {
            node_ptr = &(*node_ptr)->right;
        }
    }
    return node_ptr;
}

void
insert(Tree *tree, int val)
{
    TreeNode **place_for_val = find(tree, val);
    if (*place_for_val != NULL) {
        return;
    }
    TreeNode *new_node = malloc(sizeof(TreeNode));
    new_node->val = val;
    new_node->left = NULL;
    new_node->right = NULL;
    *place_for_val = new_node;
}


void
erase_tree(Tree *tree)
{
    void
    delete_subtree(TreeNode *node)
    {
        if (node != NULL) {
            delete_subtree(node->left);
            delete_subtree(node->right);
            free(node);
        }
    }
    delete_subtree(tree->root);
    tree->root = NULL;
}

void
print_elements(Tree tree, FILE *out)
{
    void
    print_subtree(TreeNode *node, FILE *out)
    {
        if (node != NULL) {
            print_subtree(node->right, out);
            fprintf(out, "%d ", node->val);
            print_subtree(node->left, out);
        }
    }
    print_subtree(tree.root, out);
}


int
main()
{
    Tree tree = construct_tree();
    int number;
    bool first = true;
    while (scanf("%d", &number) != EOF) {
        if (number) {
            insert(&tree, number);
        } else {
            if (first) {
                first = false;
            } else {
                fputc(' ', stdout);
            }
            print_elements(tree, stdout);
            fputc('0', stdout);
            erase_tree(&tree);
        }
    }
    if (tree.root != NULL) {
        if (!first) {
            fputc(' ', stdout);
        }
        print_elements(tree, stdout);
        fputc('0', stdout);
        erase_tree(&tree);
    }
    fputc('\n', stdout);
}

