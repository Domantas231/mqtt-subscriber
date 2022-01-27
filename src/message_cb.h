#ifndef MESSAGE_CB_H
#define MESSAGE_CB_H

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif