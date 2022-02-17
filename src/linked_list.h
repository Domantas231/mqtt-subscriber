#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "load_configs.h"

typedef struct node {
    struct node *next;

    /* big bren */ 
    union obj {
        char *string;
        struct event *ev;
        struct topic *tp;
    } obj;
} node;

void list_addback(node **head, node *new);
int list_add(node **head, node *new, int k);
void list_delall(node **head);
node* list_getid(node *head, int k);
node* list_getname(node *head, char* name);
int list_delid(node **head, int k);

#endif