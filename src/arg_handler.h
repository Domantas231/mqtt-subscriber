#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include <argp.h>

/* an arbitrary number, enough to fit long options */
#define N 30

struct arguments {
    char ca_path[N];
    char user[N];
    char pass[N];
};

extern const char doc[];
extern const char args_doc[];
extern struct argp_option options[];

error_t parse_opt (int key, char *arg, struct argp_state *state);
int parse_options(int argc, char *argv[], struct argp_option *args);

#endif