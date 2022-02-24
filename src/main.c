#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include "signal_handler.h"
#include "msg_db_handler.h"
#include "conf_mosq.h"
#include "linked_list.h"
#include "arg_handler.h"

int main(int argc, char *argv[])
{
	int rc;

	/* default options */
    struct arguments args = {.ca_path = "", .pass = "", .user = "", .use_db = 1, .port = 1883, .server = DOMAIN};
    parse_options(argc, argv, &args);

	/* open the data base to save received messages */
	if(args.use_db) open_db();

	/* load topics and events, then pass it as a void pointer */
	struct tp_node *topics = NULL;
	struct ev_node *events = NULL;
	load_topics(&topics, &events);

	/* Required before calling other mosquitto functions */
    mosquitto_lib_init();

	struct mosquitto *mosq = NULL;
	if((rc = configure_mosq(&mosq, args, topics)) != MOSQ_ERR_SUCCESS){
		goto cleanup;
	}

	/* set up signal handlers */
	setup_signals(mosq);

	/* main program loop */
	mosquitto_loop_forever(mosq, -1, 1);

	cleanup:
		syslog(LOG_INFO, "Cleaning up program");
		close_db();

		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();

		/* this is bad..? */
		list_delall_ev(&events, 1);
		list_delall_tp(&topics);

		return 0;
}