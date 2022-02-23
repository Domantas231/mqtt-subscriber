#include <syslog.h>
#include <string.h>
#include <mosquitto.h>
#include <json-c/json.h>

#include "msg_db_handler.h"
#include "linked_list.h"
#include "load_topics.h"
#include "email_handler.h"
#include "compare_msg.h"

int find_by_topic(char *topic, tp_node *topics, struct topic **tp_result){
    for(tp_node *iter = topics; iter != NULL; iter = iter->next){
        struct topic *tmp = iter->obj;

        if(strcmp(topic, tmp->name) == 0){
            *tp_result = tmp;
            return 0;
        }
    }

    syslog(LOG_WARNING, "Couldn't find event with topic %s", topic);
    return 1;
}

/*
 * TODO: Add a check for whether the string is in json format
 */
int parse_value(struct event *ev, char *payload, char *value){
    int rc = 0;
    
    json_object *jso = json_tokener_parse(payload);
    syslog(LOG_DEBUG, "Json tokener got - %s", json_object_to_json_string_ext(jso, 0));
    json_object *topic = json_object_object_get(jso, ev->param_key);

    if(topic == NULL){
        rc = 1; 
        goto json_cleanup;
    }

    char const *prsd_val = json_object_get_string(topic);
    if(strcmp(prsd_val, "null") == 0) rc = 1;

    strcpy(value, prsd_val);

    json_cleanup:
        json_object_put(jso);
        return rc;
}

int eval_events(struct topic *tp, char *payload){
    for(struct ev_node *iter = tp->events; iter != NULL; iter = iter->next){
        /* TODO: potentially is too much space for just a short? value */
        char value[256];
        parse_value(iter->obj, payload, value);

        int res = 1;
        if(iter->obj->dt == NUMBER) compare_int_msg(*iter->obj, value, &res);
        else if(iter->obj->dt == STRING) compare_str_msg(*iter->obj, value, &res);
        syslog(LOG_DEBUG, "Comparing returned %d", res);

        if(!res){
            syslog(LOG_DEBUG, "Trying to send notification email");
            send_mail(iter->obj->email_msg, iter->obj->sender, iter->obj->sender_passw, 
                      iter->obj->recp_list, 25, 0, iter->obj->email_subject);
        } 
    }
}


/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
	/* This blindly prints the payload, but the payload can be anything so take care. */
	syslog(LOG_DEBUG, "Received message - topic:%s, qos:%d, payload:%s\n", msg->topic, msg->qos, (char *)msg->payload);

    /* save message to database */
    save_message(msg->topic, msg->qos, (char *)msg->payload);

    /* get the topics from the mosq object */
    struct tp_node *topics = obj;

    /*
     * TODO: need to fix the function below
     * so that i returns a 0 on successfully
     * finding the event by topic
     * and make it compatibale with the new structure of 
     * topics 
     */ 
    
    struct topic *got_tp = NULL;
    if(find_by_topic(msg->topic, topics, &got_tp)){
        return;
    }
    syslog(LOG_DEBUG, "Found topic: %s", got_tp->name);
    
    eval_events(got_tp, (char *)msg->payload);
}