#ifndef LOAD_EVENTS_H
#define LOAD_EVENTS_H

#define CFG_NAME "mqtt_sub_events"

/*
 * TODO: recipients should be in a linked list
 * Arbitrary number, enough space to store a long topic?
 */
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
    char sender[2*N];
    char recipient[2*N];
    char sender_passw[2*N];
};

int get_events(node **events);

#endif