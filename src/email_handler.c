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
#include <string.h>
#include <curl/curl.h>
#include <syslog.h>
 
/*
 * For an SMTP example using the multi interface please see smtp-multi.c.
 */
 
/* The libcurl options want plain addresses, the viewable headers in the mail
 * can very well get a full name as well.
 */

/*
 * Probably need to fix this up a bit, so bigger messages can be stored
 * Arbitrary size numbers
 */
static char payload_header[1024];
static char payload_body[1024];
static char payload_text[2048];

static void update_payload(){
  syslog(LOG_DEBUG, "Updating the whole payload");

  snprintf(payload_text, 2048, "%s%s", payload_header, payload_body);
}

static int create_pbody(char *msg){
  syslog(LOG_DEBUG, "Updating the payload body");

  snprintf(payload_body, 1024, "%s\r\n", msg);
}

static void create_pheader(char *date, char *sndr_email, char *recpt_email, char *subject){
  syslog(LOG_DEBUG, "Updating the payload header");

  snprintf(payload_header, 1024,
  "Date: %s +1100\r\n"
  "To: A Receiver %s\r\n"
  "From: Sender Person %s\r\n"
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
  "rfcpedant.example.org>\r\n"
  "Subject: %s\r\n"
  "\r\n", /* empty line to divide headers from body, see RFC5322 */
  date, recpt_email, sndr_email, subject);
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
 
int send_mail(char *msg, char *sndr_mail, char *recpt_mail, int port, int use_ssl)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };
 
  curl = curl_easy_init();
  if(curl) {
    /*
     * TODO: 30 is arbitrary
     * need to do a bit more checking
     */

    char server_addr[30];
    sprintf(server_addr, "smtp.freesmtpservers.com:%d", port);

    /* This is the URL for your mailserver */
    curl_easy_setopt(curl, CURLOPT_URL, server_addr);
 
    /* Note that this option is not strictly required, omitting it will result
     * in libcurl sending the MAIL FROM command with empty sender data. All
     * autoresponses should have an empty reverse-path, and should be directed
     * to the address in the reverse-path which triggered them. Otherwise,
     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
     * details.
     */
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sndr_mail);

    /*
     * Use ssl verification
     */
    if(use_ssl){
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 1);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
      curl_easy_setopt(curl, CURLOPT_CAINFO , "./ca.cert");
    }

    /* Add two recipients, in this particular case they correspond to the
     * To: and Cc: addressees in the header, but they could be any kind of
     * recipient. */
    recipients = curl_slist_append(recipients, recpt_mail);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
  
    /* ================
     * MY OWN CODE
     * ================ */

    /*
     * TODO: update this for every recipient in the list
     * TODO: ALSO add a function to add recipients
     */ 

    create_pbody(msg);
    create_pheader("2021-10-10", sndr_mail, recpt_mail, "example msg");
    update_payload();

    /* ================
     * MY OWN CODE
     * ================ */

    /* We are using a callback function to specify the payload (the headers and
     * body of the message). You could just use the CURLOPT_READDATA option to
     * specify a FILE pointer to read from. */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* Send the message */
    res = curl_easy_perform(curl);
 
    /* Check for errors */
    if(res != CURLE_OK)
      syslog(LOG_ERR, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Free the list of recipients */
    curl_slist_free_all(recipients);
 
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