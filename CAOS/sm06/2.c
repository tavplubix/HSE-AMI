#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct ListNode
{
    struct ListNode *prev;
    struct ListNode *next;
    int number;
    uint32_t count;
} ListNode;

typedef struct List
{
    ListNode *first;
    ListNode *last;
} List;

List
construct_list()
{
    List l;
    l.first = NULL;
    l.last = NULL;
    return l;
}

void
push_front(List *list, int number)
{
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->number = number;
    new_node->count = 1;
    new_node->prev = NULL;
    new_node->next = list->first;
    if (new_node->next != NULL) {
        new_node->next->prev = new_node;
    }
    list->first = new_node;
    if (list->last == NULL) {
        list->last = new_node;
    }
}

ListNode *
find(List list, int number)
{
    ListNode *node = list.first;
    while (node != NULL && node->number != number) {
        node = node->next;
    }
    return node;
}

void
move_front(List *list, ListNode *node)
{
    if (list->first == node) {
        return;
    }
    node->prev->next = node->next;
    if (list->last == node) {
        list->last = node->prev;
    } else {
        node->next->prev = node->prev;
    }

    node->prev = NULL;
    node->next = list->first;
    node->next->prev = node;
    list->first = node;
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
        ListNode *n = find(list, number);
        if (n == NULL) {
            push_front(&list, number);
        } else {
            ++n->count;
            move_front(&list, n);
        }
    }
    const ListNode *node = list.last;
    while (node != NULL) {
        printf("%d %"PRIu32"\n", node->number, node->count);
        node = node->prev;
    }
    destruct_list(list);
}

