#ifndef EMAIL_HANDLER_H
#define EMAIL_HANDLER_H

#include "load_topics.h"

/*
 * Keep in mind the message size has to be under 2048?
 * maybe need to change that
 */
int send_mail(struct event *ev, int port, int use_ssl);

#endif