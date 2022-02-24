#ifndef CONF_EMSG_H
#define CONF_EMSG_H

/*
 * Probably need to fix this up a bit, so bigger messages can be stored
 * Arbitrary size numbers
 */
#define MAX_N 2048

extern char payload_header[MAX_N];
extern char payload_body[MAX_N];
extern char payload_text[2*MAX_N];

void update_payload();
int create_pbody(char *msg);
void create_pheader(char *sndr_email, char *recipient, char *subject);

#endif