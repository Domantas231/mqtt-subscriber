#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "load_configs.h"

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

void list_delall_ev(ev_node **head){
    ev_node *temp;

    while(*head != NULL){
        temp = *head;
        *head = (*head)->next;

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

        free(temp);
    }
}