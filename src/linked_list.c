#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "load_topics.h"
#include "linked_list.h"

/* 
 * TODO: There has to be a better way to do this
 */

void list_addback_ev(ev_node **head, ev_node *new){
    if(*head == NULL){
        *head = new;
        return;
    }

    ev_node *iter = *head;
    while(iter->next != NULL) iter = iter->next;

    iter->next = new;
}

void list_delall_ev(ev_node **head, int free_str){
    ev_node *temp;

    while(*head != NULL){
        temp = *head;
        *head = (*head)->next;

        /* only adding this one line because I HAVE to free all of the events in a different line */
        /* otherwise I get a segmentation fault trying to free a space that has already been freed */
        if(free_str) list_delall_str(&temp->obj->recp_list);

        free(temp->obj);
        free(temp);
    }
}

void list_addback_tp(tp_node **head, tp_node *new){
    if(*head == NULL){
        *head = new;
        return;
    }

    tp_node *iter = *head;
    while(iter->next != NULL) iter = iter->next;

    iter->next = new;
}

void list_delall_tp(tp_node **head){
    tp_node *temp;

    while(*head != NULL){
        temp = *head;
        *head = (*head)->next;

        list_delall_ev(&temp->obj->events, 0);
        free(temp->obj);
        free(temp);
    }
}

void list_addback_str(str_node **head, str_node *new){
    if(*head == NULL){
        *head = new;
        return;
    }

    str_node *iter = *head;
    while(iter->next != NULL) iter = iter->next;

    iter->next = new;
}

void list_delall_str(str_node **head){
    str_node *temp;

    while(*head != NULL){
        temp = *head;
        *head = (*head)->next;

        free(temp->obj);
        free(temp);
    }
}