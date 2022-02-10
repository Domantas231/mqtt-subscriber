#ifndef CONF_MOSQ_H
#define CONF_MOSQ_H

#include <mosquitto.h>

#define DOMAIN "test.mosquitto.org"

int configure_mosq(struct mosquitto *mosq, int argc, char *argv[]);

#endif