#ifndef CONF_MOSQ_H
#define CONF_MOSQ_H

#include <mosquitto.h>
#include "load_configs.h"

#define DOMAIN "test.mosquitto.org"

int configure_mosq(struct mosquitto *mosq, int argc, char *argv[], struct tp_node *topics);

#endif