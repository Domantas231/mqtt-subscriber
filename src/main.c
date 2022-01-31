#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include "signal_handler.h"
#include "connect_cb.h"
#include "subscribe_cb.h"
#include "message_cb.h"
#include "msg_db_handler.h"
#include "arg_handler.h"

#define DOMAIN "test.mosquitto.org"

int main(int argc, char *argv[])
{
	int rc;

	/*
	 * Parse the given program options
	 */
	syslog(LOG_DEBUG, "Parsing options");
    struct arguments args;
    struct argp argp = {options, parse_opt, args_doc, doc};
    argp_parse(&argp, argc, argv, 0, 0, &args);

	/*
	 * Open the database used for saving
	 * received messages
	 */
	open_db();

	/*
	 * Setting up signal handlers
	 */
	setup_signals();

	/* Required before calling other mosquitto functions */
	struct mosquitto *mosq;
	mosquitto_lib_init();

	/* Create a new client instance.
	 * id = NULL -> ask the broker to generate a client id for us
	 * clean session = true -> the broker should remove old sessions when we connect
	 * obj = NULL -> we aren't passing any of our private data for callbacks
	 */
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		syslog(LOG_ERR, "Out of memory, failed to start mosquitto context.\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * Default port for unauthenticated, uncencrypted
	 * test.mosquitto.org broker
	 * (this will change if TLS or authentication is specified)
	 */
	int port = 1883;

	/*
	 * Configure TLS 
	 * (if the necessary options are passed)
	 */
	if(args.ca_path != NULL){
		syslog(LOG_DEBUG, "Trying to configure TLS.");
		rc = mosquitto_tls_set(mosq, args.ca_path, NULL, NULL, NULL, NULL);
		port = 1883;

		if(rc != MOSQ_ERR_SUCCESS){
			syslog(LOG_ERR, "Failed to setup TLS: %s", mosquitto_strerror(rc));
			goto cleanup;
		}
	}

	// /*
	//  * Configure authentication
	//  * (if the necessary options are passed)
	//  */
	// if(args.user != NULL && args.pass != NULL){
	// 	syslog(LOG_DEBUG, "Trying to configure authentication");
	//	port = port == 8883 ? 8885 : 1884;
	//
	// 	mosquitto_username_pw_set(mosq, args.user, args.pass);
	// }


	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

	/* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
	 * This call makes the socket connection only, it does not complete the MQTT
	 * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
	 * mosquitto_loop_forever() for processing net traffic. */
	rc = mosquitto_connect(mosq, DOMAIN, port, 60);
	if(rc != MOSQ_ERR_SUCCESS){;
		syslog(LOG_ERR, "Failed to connect to domain: %s\n", mosquitto_strerror(rc));
		goto cleanup;
	}

	/* Run the network loop in a blocking call. The only thing we do in this
	 * example is to print incoming messages, so a blocking call here is fine.
	 *
	 * This call will continue forever, carrying automatic reconnections if
	 * necessary, until the user calls mosquitto_disconnect().
	 */
	mosquitto_loop_start(mosq);


	while(daemonise){
		// TODO: do something
	}

	/*
	 * TODO: Check whether force is really needed 
	 */
	mosquitto_loop_stop(mosq, true);

	syslog(LOG_INFO, "Cleaning up program");
	/*
	 * TODO: Change so it isn't so open 
	 */
	close_db();
	mosquitto_lib_cleanup();
	exit(EXIT_SUCCESS);

	cleanup:
		close_db();

		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		exit(EXIT_FAILURE);
}