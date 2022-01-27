#include <syslog.h>
#include <mosquitto.h>

#include "msg_db_handler.h"

/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	/* This blindly prints the payload, but the payload can be anything so take care. */
	syslog(LOG_DEBUG, "Received message - topic:%s, qos:%d, payload:%s\n", msg->topic, msg->qos, (char *)msg->payload);

    /*
     * Save the message to a database
     */
    save_message(msg->topic, msg->qos, (char *)msg->payload);
}
