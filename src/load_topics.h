#ifndef LOAD_TOPICS_H
#define LOAD_TOPICS_H

#define CFG_NAME "mqtt_sub_topics"

#include "linked_list.h"

struct topic {
    char *name;
    int qos;
};

int get_topics(node **topics);

#endif
