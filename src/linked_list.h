#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "load_topics.h"

void list_addback_ev(ev_node **head, ev_node *new);
void list_addback_tp(tp_node **head, tp_node *new);
void list_addback_str(str_node **head, str_node *new);
void list_delall_ev(ev_node **head, int free_str);
void list_delall_tp(tp_node **head);
void list_delall_str(str_node **head);

#endif