#include <sqlite3.h>
#include <time.h>
#include <syslog.h>
#include <stdio.h>

#define FD "mqtt_sub_msgs.db"

sqlite3 *DB = NULL;

/*
 * Get the current time
 */
int curr_time(char current[], int n){
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(current, n, "%Y-%m-%d %H:%M:%S\n", timeinfo);

    return 0;
}

/*
 * Need to convert payload to char* first
 */
int save_message(char *topic, int qos, char *payload){
    int rc;

    char *err;

    char time[26];
    curr_time(time, 26);

    char *temp = "INSERT INTO Messages (time, topic, qos, payload) VALUES";

    char query[128];
    snprintf(query, 128, "%s('%s', '%s', '%d', '%s')", temp, time, topic, qos, payload);

    rc = sqlite3_exec(DB, query, NULL, NULL, &err);
    if(rc != SQLITE_OK)
        syslog(LOG_WARNING, "Can't insert new message: %s \n", err);
        
    return 0;
}

static int create_db(){
    int rc;

    char *err;
    
    char *table = "CREATE TABLE if not exists Messages("
                        "id INTEGER PRIMARY KEY,"
                        "time TEXT NOT NULL,"
                        "topic TEXT NOT NULL,"
                        "qos INTEGER NOT NULL,"
                        "payload TEXT NOT NULL )";

    rc = sqlite3_exec(DB, table, NULL, NULL, &err);
    if(rc != SQLITE_OK)
        syslog(LOG_WARNING, "Can't create table/database: %s \n", err);

    return rc;
}

int open_db(){
    syslog(LOG_INFO, "Opening database used for saving messages");

    int rc;
    if((rc = sqlite3_open(FD, &DB)) != SQLITE_OK)
        syslog(LOG_WARNING, "Failed to open database, rc=%d", rc);

    create_db();

    return rc;
}

int close_db(){
    syslog(LOG_INFO, "Closing database used for saving messages");

    int rc;
    if((rc = sqlite3_close(DB)) != SQLITE_OK)
        syslog(LOG_WARNING, "Failed to close database, rc=%d", rc);

    return rc;
}