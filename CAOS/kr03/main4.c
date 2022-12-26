#include<stdio.h>
#include <stdlib.h>

typedef struct Node
{
    signed char value;
    struct Node *next;
} Node;


void push_front(Node** head, signed char value) {
    Node* new = malloc(sizeof(Node));
    new->value = value;
    new->next = *head;
    *head = new;
}

extern struct Node *list_del(struct Node *head, int value);

int main() {
    int n;
    scanf("%d", &n);
    Node* list = NULL;
    for (int i = 0; i < n; ++i) {
        signed char val;
        scanf("%c", &val);
        push_front(&list, val);
    }
    signed char val;
    scanf("%c", &val);
    
    Node* _list = list;

    while (list) {
        printf("'%c' ", list->value);
        list = list->next;
    }
    printf("\n");

    list = list_del(_list, val);

    while (list) {
        printf("'%c' ", list->value);
        list = list->next;
    }
    printf("\n");

}





