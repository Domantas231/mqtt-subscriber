#ifndef CONF_MOSQ_H
#define CONF_MOSQ_H

#include <mosquitto.h>
#include "load_topics.h"
#include "arg_handler.h"

#define DOMAIN "test.mosquitto.org"

int configure_mosq(struct mosquitto **mosq, struct arguments args, struct tp_node *topics);

#endif