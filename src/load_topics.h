#ifndef LOAD_TOPICS_H
#define LOAD_TOPICS_H

#define CFG_NAME "mqtt_sub_topics"

struct topic {
    char *name;
    int qos;
};

// int get_topics(struct topic topics[], int t_num);

#endif