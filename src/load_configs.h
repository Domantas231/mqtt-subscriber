#ifndef LOAD_CONFIGS_H
#define LOAD_CONFIGS_H

#define TOPIC_CFG "mqtt_sub_topics"
#define EVENT_CFG "mqtt_sub_events"

#include "linked_list.h"

/* ======================
 * Event related loading
 * ====================== */

/* Might not be big enough */
#define N 50

enum data_type {
    STRING,
    NUMBER
};

typedef struct str_node {
    struct str_node *next;

    char *obj;
} str_node;

struct event {
    char topic[N];
    char param_key[N];
    enum data_type dt;
    char value[N];
    char compare[2];
    str_node *recp_list;
    char sender[2*N];
    char sender_passw[2*N];
    char email_msg[2*N];
    char email_subject[N];
};

typedef struct ev_node {
    struct ev_node *next;

    struct event obj;
} ev_node;

/* ======================
 * Topic related loading
 * ====================== */

struct topic {
    char name[N];
    int qos;
    ev_node *events;
};

typedef struct tp_node {
    struct tp_node *next;

    struct topic obj;
} tp_node;

/* TODO: fix this mess */
void list_addback_ev(ev_node **head, ev_node *new);
void list_addback_tp(tp_node **head, tp_node *new);
void list_addback_str(str_node **head, str_node *new);
void list_delall_ev(ev_node **head);
void list_delall_tp(tp_node **head);
void list_delall_str(str_node **head);

int load_topics(struct tp_node **head);

#endif