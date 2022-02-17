#include <syslog.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>

#include "connect_cb.h"
#include "load_configs.h"
#include "linked_list.h"

int subscribe_every_topic(struct mosquitto *mosq, tp_node *topics){
	/* TODO: this rc is tricky */
	int rc = 0;

	/* Send all of the topics/subscribe packets to the broker */
	for(tp_node *iter = topics; iter != NULL; iter = iter->next){
		/* 
		* Making subscriptions in the on_connect() callback means that if the
		* connection drops and is automatically resumed by the client, then the
		* subscriptions will be recreated when the client reconnects. --- VERY GOOD
		*/
        struct topic *tpcm = &iter->obj;

		syslog(LOG_INFO, "Trying to subscribe to %s with qos %d", tpcm->name, tpcm->qos);
		rc = mosquitto_subscribe(mosq, NULL, tpcm->name, tpcm->qos);

		if(rc != MOSQ_ERR_SUCCESS){
			syslog(LOG_ERR, "Error subscribing: %s\n", mosquitto_strerror(rc));
		}
	}

    list_delall_tp(&topics);
	return rc;
}

/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int rc){
	syslog(LOG_DEBUG, "on_connect: %s\n", mosquitto_connack_string(rc));

	if(rc != 0){
		/* If the connection fails for any reason, we don't want to keep on
		 * retrying in this example, so disconnect. Without this, the client
		 * will attempt to reconnect. */
		syslog(LOG_WARNING, "Failed to connect to the broker");
		mosquitto_disconnect(mosq);
	}

	struct tp_node *topics = (tp_node *)obj;
	subscribe_every_topic(mosq, topics);
}