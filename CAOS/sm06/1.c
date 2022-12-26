#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode
{
    struct ListNode *next;
    int val;
} ListNode;

typedef struct List
{
    ListNode *first;
} List;

List
construct_list()
{
    List l;
    l.first = NULL;
    return l;
}

void
push_front(List *list, int elem)
{
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->val = elem;
    new_node->next = list->first;
    list->first = new_node;
}

void
destruct_list(List list)
{
    ListNode *node = list.first;
    while (node != NULL) {
        ListNode *cur = node;
        node = cur->next;
        free(cur);
    }
}


int
main()
{
    List list = construct_list();
    int number;
    while (scanf("%d", &number) != EOF) {
        push_front(&list, number);
    }
    const ListNode *node = list.first;
    while (node != NULL) {
        printf("%d\n", node->val);
        node = node->next;
    }
    destruct_list(list);
}

