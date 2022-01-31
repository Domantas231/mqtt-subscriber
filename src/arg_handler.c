#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "arg_handler.h"

const char doc[] = "A simple program that adds mqtt subscriber functionality to the router.";

const char args_doc[] = "";

struct argp_option options[] = {
  {"ca-file", 'c', "FILE", 0, "Supply path to CA file, to use TLS encryption" },
  {"username", 'u', "STRING", 0, "Needed for client authentication (as well as password)"},
  {"password", 'p', "STRING", 0, "Needed for client authentication (as well as username)"},
  { 0 }
};

error_t parse_opt (int key, char *arg, struct argp_state *state){
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments *args = state->input;

    switch (key)
    {
    case 'c':
        strncpy(args->ca_path, arg, N);
        break;

    case 'u':
        strncpy(args->user, arg, N);
        break;

    case 'p':
        strncpy(args->pass, arg, N);
        break;

    default:
        syslog(LOG_WARNING, "No options with keycode %c were passed.", key);
        return 0;
    }

    return 0;
}