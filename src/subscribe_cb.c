#include <mosquitto.h>
#include <syslog.h>


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	bool have_subscription = false;

	/* In this example we only subscribe to a single topic at once, but a
	 * SUBSCRIBE can contain many topics at once, so this is one way to check
	 * them all. */
	for(int i = 0; i < qos_count; i++){
		syslog(LOG_DEBUG, "on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);

		if(granted_qos[i] <= 2){
			syslog(LOG_DEBUG, "Successfully subscribed to \"%s\" topic", "i dont know");
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		/* 
		 * The broker rejected all of our subscriptions.
		 * There's no point in being connected. 
		 */
		syslog(LOG_ERR, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}
