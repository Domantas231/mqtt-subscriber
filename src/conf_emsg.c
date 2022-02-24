#include <syslog.h>
#include <stdio.h>

#include "msg_db_handler.h"
#include "conf_emsg.h"

char payload_header[MAX_N];
char payload_body[MAX_N];
char payload_text[2*MAX_N];

void update_payload(){
  syslog(LOG_DEBUG, "Updating the whole payload");

  snprintf(payload_text, 2*MAX_N, "%s%s", payload_header, payload_body);
}

int create_pbody(char *msg){
  syslog(LOG_DEBUG, "Updating the payload body");

  snprintf(payload_body, MAX_N, "%s\r\n", msg);
}

void create_pheader(char *sndr_email, char *recipient, char *subject){
  syslog(LOG_DEBUG, "Updating the payload header");

  char time[26];
  curr_time(time, 26);

  snprintf(payload_header, MAX_N,
  "Date: %s +1100\r\n"
  "To: %s\r\n"
  "From: %s\r\n"
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@rfcpedant.example.org>\r\n"
  "Subject: %s\r\n"
  "\r\n", /* empty line to divide headers from body, see RFC5322 */
  time, recipient, sndr_email, subject);
}