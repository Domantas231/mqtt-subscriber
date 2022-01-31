#include <syslog.h>
#include <mosquitto.h>

#include "msg_db_handler.h"
#include "linked_list.h"
#include "load_events.h"

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
    validate_events();

    struct event got_ev; 
    if(!find_by_topic(msg->topic, events, &got_ev)){
        return;
    }

    /*
     * TODO: Not very safe i cry
     */ 
    void *value = NULL;
    parse_value(got_ev, (char *)msg->payload, value);

    compare_msg(got_ev, value);

    send_notice();

    list_delall(&events);   
}

int find_by_topic(char *topic, node *events, struct event *ev_result){
    for(node *iter = events; iter != NULL; iter = iter->next){
        struct event tmp = *(struct event *)iter->obj;

        if(strcmp(topic, tmp.topic) == 0){
            ev_result = &tmp;
            return 0; 
        }
    }

    syslog(LOG_WARNING, "Couldn't find event with topic %s", topic);
    return 1;
}

int parse_value(struct event ev, char *payload, void *value){

}

int compare_msg(struct event ev, void *value){
    /*
     * TODO: highly likely this can be done with 
     * a switch case where there are nested cases
     */

    /*
     * TODO: Need to check whether this is a string
     * or a number so that the later comparisons don't 
     * do something funny
     */
    if(strcmp(ev.compare, ">") == 0){
        if(*(int *)value > ev.value){
            return 0;
        }

        return 1;
    }
    else if(strcmp(ev.compare, "<") == 0){

    }
    else if(strcmp(ev.compare, "==") == 0){

    }
    else if(strcmp(ev.compare, "!=") == 0){

    }
    else if(strcmp(ev.compare, "<=") == 0){

    }
    else if(strcmp(ev.compare, ">=") == 0){

    }

    /*
     * TODO: Could add checking here is compare is a right value 
     */
}