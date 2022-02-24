/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2021, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
 
/* <DESC>
 * Send email with SMTP
 * </DESC>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <syslog.h>
 
#include "linked_list.h"
#include "msg_db_handler.h"

/*
 * Probably need to fix this up a bit, so bigger messages can be stored
 * Arbitrary size numbers
 */
#define MAX_N 2048

static char payload_header[MAX_N];
static char payload_body[MAX_N];
static char payload_text[2*MAX_N];

static void update_payload(){
  syslog(LOG_DEBUG, "Updating the whole payload");

  snprintf(payload_text, 2*MAX_N, "%s%s", payload_header, payload_body);
}

static int create_pbody(char *msg){
  syslog(LOG_DEBUG, "Updating the payload body");

  snprintf(payload_body, MAX_N, "%s\r\n", msg);
}

static void create_pheader(char *sndr_email, char *recipient, char *subject){
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

struct upload_status {
  size_t bytes_read;
};
 
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

int send_mail(struct event *ev, int port, int use_ssl)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };
 
  curl = curl_easy_init();
  if(curl) {
    /* TODO: 50 is arbitrary */
    /* TODO: the user might need to specify this n other stuuuff */
    char server_addr[50];
    snprintf(server_addr, 50, "smtp.mailgun.org:%d", port);
    curl_easy_setopt(curl, CURLOPT_URL, server_addr);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ev->sender);
    curl_easy_setopt(curl, CURLOPT_USERNAME, ev->sender);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, ev->sender_passw);

    /* if using ssl verification */ 
    if(use_ssl){
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 1);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
      curl_easy_setopt(curl, CURLOPT_CAINFO , "./ca.cert");
    }

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  
    /*
     * TODO: update this for every recipient in the list
     * update header for every recipient 
     */

    for(str_node *iter = ev->recp_list; iter != NULL; iter = iter->next){
      struct curl_slist *recipient = NULL;
      recipient = curl_slist_append(recipient, iter->obj);

      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipient);

      create_pbody(ev->email_msg);
      create_pheader(ev->sender, iter->obj, ev->email_subject);
      update_payload();

      /* Send the message */
      res = curl_easy_perform(curl);

      /* Check for errors */
      if(res != CURLE_OK)
        syslog(LOG_ERR, "curl_easy_perform() failed: %d\n", res);

      /* Free the list of recipients */
      curl_slist_free_all(recipients);
    }
 
    /* curl will not send the QUIT command until you call cleanup, so you
     * should be able to re-use this connection for additional messages
     * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
     * calling curl_easy_perform() again. It may not be a good idea to keep
     * the connection open for a very long time though (more than a few
     * minutes may result in the server timing out the connection), and you do
     * want to clean up in the end.
     */
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}