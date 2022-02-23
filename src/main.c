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

int main(int argc, char *argv[])
{
	int rc;

	/* open the data base to save received messages */
	/* TODO: do they really want this */
	open_db();

	/* load topics and events, then pass it as a void pointer */
	/* TODO: waht if theydotn exsit */
	struct tp_node *topics = NULL;
	load_topics(&topics);

	/* Required before calling other mosquitto functions */
    mosquitto_lib_init();

	struct mosquitto *mosq = NULL;
	if((rc = configure_mosq(&mosq, argc, argv, topics)) != MOSQ_ERR_SUCCESS){
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

		list_delall_tp(&topics);

		return 0;
}