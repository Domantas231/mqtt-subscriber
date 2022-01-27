#ifndef SUBSCRIBE_CB_H
#define SUBSCRIBE_CB_H

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);

#endif