#include <syslog.h>
#include <string.h>

#include "conf_mosq.h"
#include "arg_handler.h"
#include "connect_cb.h"
#include "subscribe_cb.h"
#include "message_cb.h"

int configure_mosq(struct mosquitto **mosq, struct arguments args, struct tp_node *topics){
    int rc = 0;

    /* Create a new client instance. */
	*mosq = mosquitto_new(NULL, true, topics);
	if(*mosq == NULL){
		syslog(LOG_ERR, "Out of memory, failed to start mosquitto context.\n");
		return 1;
	}

	/* Configure TLS, if needed */
	if(strcmp(args.ca_path, "") != 0){
		syslog(LOG_DEBUG, "Trying to configure TLS.");
		rc = mosquitto_tls_set(*mosq, args.ca_path, NULL, NULL, NULL, NULL);
		args.port = 8883;

		if(rc != MOSQ_ERR_SUCCESS){
			syslog(LOG_ERR, "Failed to setup TLS: %s", mosquitto_strerror(rc));
			return rc;
		}
	}

	/* Configure authentication, if needed */
	if(strcmp(args.user, "") != 0 && strcmp(args.pass, "") != 0){
		syslog(LOG_DEBUG, "Trying to configure authentication");
		args.port = args.port == 8883 ? 8884 : 1884;
	
		mosquitto_username_pw_set(*mosq, args.user, args.pass);
	}

	syslog(LOG_DEBUG, "Current port is %d", args.port);

	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_connect_callback_set(*mosq, on_connect);
	mosquitto_subscribe_callback_set(*mosq, on_subscribe);
	mosquitto_message_callback_set(*mosq, on_message);

    rc = mosquitto_connect(*mosq, args.server, args.port, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		syslog(LOG_ERR, "Failed to connect to domain: %s\n", mosquitto_strerror(rc));
		return rc;
	}

    return rc;
}