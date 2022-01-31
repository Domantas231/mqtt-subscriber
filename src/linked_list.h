#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
    /*
     * TODO: How safe is this??
     */
    void *obj;

    struct node *next; 
}node;

void list_addback(node **head, node *new);
int list_add(node **head, node *new, int k);
void list_delall(node **head);
node* list_getid(node *head, int k);
node* list_getname(node *head, char* name);
int list_delid(node **head, int k);

#endif