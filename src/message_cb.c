#include <syslog.h>
#include <string.h>
#include <mosquitto.h>
#include <json-c/json.h>

#include "msg_db_handler.h"
#include "linked_list.h"
#include "load_events.h"
#include "email_handler.h"

int find_by_topic(char *topic, node *events, struct event **ev_result){
    for(node *iter = events; iter != NULL; iter = iter->next){
        struct event *tmp = (struct event *)iter->obj;

        if(strcmp(topic, tmp->topic) == 0){
            ev_result = &tmp;
            return 0;
        }
    }

    syslog(LOG_WARNING, "Couldn't find event with topic %s", topic);
    return 1;
}

/*
 * TODO: Add a check for whether the string is in json format
 */
int parse_value(struct event ev, char *payload, void *value){
    int rc = 0;
    
    json_object *jso = json_tokener_parse(payload);

    json_object *topic = json_object_object_get(jso, ev.topic);

    char const *prsd_val = json_object_to_json_string_ext(topic, 0);
    if(strcmp(prsd_val, "null") == 0){
        rc = 1;
    }

    value = prsd_val;

    json_object_put(jso);
    return rc;
}

/*
 * TODO: Make a separate function for ints and strings
 */

int compare_int_msg(struct event ev, void *value, int *res){
    if(strcmp(ev.compare, ">") == 0){
        if(*(int *)value > atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else if(strcmp(ev.compare, "<") == 0){
        if(*(int *)value < atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else if(strcmp(ev.compare, "==") == 0){
        if(*(int *)value == atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else if(strcmp(ev.compare, "!=") == 0){
        if(*(int *)value =! atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else if(strcmp(ev.compare, "<=") == 0){
        if(*(int *)value <= atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else if(strcmp(ev.compare, ">=") == 0){
        if(*(int *)value >= atoi(ev.value)){
            *res = 0;
        }

        *res = 1;
    }
    else {
        return 1;
    }

    return 0;
}

/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	/* This blindly prints the payload, but the payload can be anything so take care. */
	syslog(LOG_DEBUG, "Received message - topic:%s, qos:%d, payload:%s\n", msg->topic, msg->qos, (char *)msg->payload);

    /*
     * Save the message to a database
     */
    save_message(msg->topic, msg->qos, (char *)msg->payload);

    /*
     * Parse the events saved in a specific config file
     */
    node *events = NULL;
    get_events(&events);

    /*
     * TODO: Check whether things like compare "<" two strings
     * doesn't happen, but I don't know if this is truly needed
     * and if it wouldn't be better to have it in a different spot
     */

    struct event *got_ev = NULL;
    if(!find_by_topic(msg->topic, events, &got_ev)){
        return;
    }

    syslog(LOG_DEBUG, "Found event: %s, with value %s", got_ev->topic, got_ev->value);

    /*
     * TODO: Not very safe i cry
     */ 
    void *value = NULL;
    parse_value(*got_ev, (char *)msg->payload, value);

    int res;
    compare_int_msg(*got_ev, value, &res);

    syslog(LOG_DEBUG, "Comparing returned %d", res);
    if(res){
        syslog(LOG_DEBUG, "Trying to send notification email");
        send_mail("IT WORKS!!!", got_ev->sender, got_ev->recipient, 25, 0);
    }

    list_delall(&events);
}