#include <mosquitto.h>
#include <syslog.h>

#include "load_topics.h"


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	bool have_subscription = false;

	tp_node *iter = obj;
	for(int i = 0; i < qos_count; i++){
		syslog(LOG_DEBUG, "on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
	
		if(granted_qos[i] <= 2){
			syslog(LOG_DEBUG, "Successfully subscribed to \"%s\" topic", iter->obj->name);
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		/* 
		 * The broker rejected all of our subscriptions.
		 * There's no point in being connected. 
		 */
		syslog(LOG_ERR, "All subscriptions rejected, disconnecting.\n");
		mosquitto_disconnect(mosq);
	}
}
