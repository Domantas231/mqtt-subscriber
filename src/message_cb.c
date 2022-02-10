#include <syslog.h>
#include <string.h>
#include <mosquitto.h>
#include <json-c/json.h>

#include "msg_db_handler.h"
#include "linked_list.h"
#include "load_configs.h"
#include "email_handler.h"

int find_by_topic(char *topic, node *events, struct event **ev_result){
    for(node *iter = events; iter != NULL; iter = iter->next){
        struct event *tmp = (struct event *)iter->obj;

        if(strcmp(topic, tmp->topic) == 0){
            *ev_result = tmp;
            return 0;
        }
    }

    syslog(LOG_WARNING, "Couldn't find event with topic %s", topic);
    return 1;
}

/*
 * TODO: Add a check for whether the string is in json format
 */
int parse_value(struct event ev, char *payload, char *value){
    int rc = 0;
    
    json_object *jso = json_tokener_parse(payload);
    syslog(LOG_DEBUG, "Tokener got - %s", json_object_to_json_string_ext(jso, 0));
    json_object *topic = json_object_object_get(jso, ev.param_key);

    /*
     * TODO: Fix this error checking, cause it dont work
     */
    char const *prsd_val = json_object_to_json_string_ext(topic, 0);
    if(prsd_val == NULL){
        rc = 1;
    }

    strcpy(value, prsd_val);

    json_object_put(jso);
    return rc;
}

int compare_str_msg(struct event ev, char *value, int *res){
    if(strcmp(ev.compare, "==") == 0){
        if(strcmp(ev.value, value) == 0){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, "!=") == 0){
        if(strcmp(ev.value, value) == 0){
            *res = 0;
        }
        else
            *res = 1;
    }

    return 0;
}

/*
 * TODO: Make a separate function for ints and strings
 */
int compare_int_msg(struct event ev, char *value, int *res){
    /*
     * TODO: Fix this horrendous mess
     */
    
    if(strcmp(ev.compare, ">") == 0){
        if(atoi(value) > atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, "<") == 0){
        if(atoi(value) < atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, "==") == 0){
        if(atoi(value) == atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, "!=") == 0){
        if(atoi(value) != atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, "<=") == 0){
        if(atoi(value) <= atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else if(strcmp(ev.compare, ">=") == 0){
        if(atoi(value) >= atoi(ev.value)){
            *res = 0;
        }
        else
            *res = 1;
    }
    else {
        return 1;
    }

    return 0;
}

/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
	/* This blindly prints the payload, but the payload can be anything so take care. */
	syslog(LOG_DEBUG, "Received message - topic:%s, qos:%d, payload:%s\n", msg->topic, msg->qos, (char *)msg->payload);

    /* save message to database */
    save_message(msg->topic, msg->qos, (char *)msg->payload);

    /* TODO: change so that I don't need to parse this every time a message comes */
    node *events = NULL;
    get_events(&events);

    /*
     * TODO: Check whether things like compare "<" two strings
     * doesn't happen, but I don't know if this is truly needed
     * and if it wouldn't be better to have it in a different spot
     */

    struct event *got_ev = NULL;

    /*
     * TODO: need to fix the function below
     * so that i returns a 0 on successfully
     * finding the event by topic
     */ 

    if(find_by_topic(msg->topic, events, &got_ev)){
        return;
    }
    syslog(LOG_DEBUG, "Found event: %s, with value %s", got_ev->topic, got_ev->value);
    
    /* 
     * TODO: change maybe
     */
    char value[256];
    parse_value(*got_ev, (char *)msg->payload, value);

    int res;
    compare_int_msg(*got_ev, value, &res);

    /*
     * 26 is the exact number of characters
     * in a string that contains the time/date.
     */
    char time[26];
    curr_time(time, 26);

    syslog(LOG_DEBUG, "Comparing returned %d", res);
    if(!res){
        syslog(LOG_DEBUG, "Trying to send notification email");
        send_mail("Warning: something:)", got_ev->sender, got_ev->sender_passw, got_ev->recp_list, 25, 0, time, "example message");
    }

    list_delall(&events);
}