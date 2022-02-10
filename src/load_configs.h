#ifndef LOAD_CONFIGS_H
#define LOAD_CONFIGS_H

#define TOPIC_CFG "mqtt_sub_topics"
#define EVENT_CFG "mqtt_sub_events"

#include "linked_list.h"

/* ======================
 * Topic related loading
 * ====================== */

struct topic {
    char *name;
    int qos;
};

/* ======================
 * Event related loading
 * ====================== */

/* Might not be big enough */
#define N 50

enum data_type {
    STRING,
    NUMBER
};

struct event {
    char topic[N];
    char param_key[N];
    enum data_type dt;
    char value[N];
    char compare[2];
    node* recp_list;
    char sender[2*N];
    char sender_passw[2*N];
};


int parse_config_data(node **events, node **topics);

#endif