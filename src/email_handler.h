#ifndef EMAIL_HANDLER_H
#define EMAIL_HANDLER_H

/*
 * TODO: Keep in mind the message size has to be under 1024
 * maybe need to change that
 */
int send_mail(char *msg, char *sndr_mail, char* sndr_passw, node *recpt_list, int port, int use_ssl, char* time, char* subject);

#endif