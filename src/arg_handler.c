#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

#include "arg_handler.h"

const char doc[] = "A simple program that adds mqtt subscriber functionality to the router.";

const char args_doc[] = "";

struct argp_option options[] = {
  {"ca-file", 'c', "FILE", 0, "Supply path to CA file, needed if using TLS encryption" },
  {"username", 'u', "STRING", 0, "If using client authentication (need to specify password as well)"},
  {"password", 'k', "STRING", 0, "If using client authentication (need to specify username as well)"},
  {"port", 'p', "INT", 0, "Specify to override the default port (1883, 1884 or 8883, 8884)"},
  {"broker", 'b', "STRING", 0, "Specify the broker to connect to (default is test.mosquitto.org)"},
  {"dont-use-db", 'd', 0, 0, "Specify if saving messages to a database is not required"},
  { 0 }
};

error_t parse_opt (int key, char *arg, struct argp_state *state){
    struct arguments **args = state->input;

    switch (key)
    {
    case 'c':
        strncpy((*args)->ca_path, arg, Nopt);
        break;

    case 'u':
        strncpy((*args)->user, arg, Nopt);
        break;

    case 'k':
        strncpy((*args)->pass, arg, Nopt);
        break;

    case 'p':
        (*args)->port = strtol(arg, NULL, 10);
    
    case 'b':
        strncpy((*args)->server, arg, Nopt);

    case 'd':
        (*args)->use_db = 0;

    default:
        return 0;
    }

    return 0;
}

int parse_options(int argc, char *argv[], struct arguments *args){
	syslog(LOG_DEBUG, "Parsing options");
    struct argp argp = {options, parse_opt, NULL, doc};
    argp_parse(&argp, argc, argv, 0, 0, &args);
}