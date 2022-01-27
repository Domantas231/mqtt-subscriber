#ifndef MSG_DB_HANDLER_H
#define MSG_DB_HANDLER_H

int open_db();
int save_message(char *topic, int qos, char *payload);
int close_db();

#endif