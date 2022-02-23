#ifndef COMPARE_MSG_H
#define COMPARE_MSG_H

#include "load_topics.h"

int compare_str_msg(struct event ev, char *value, int *res);
int compare_int_msg(struct event ev, char *value, int *res);

#endif