#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <uci.h>

#include "load_configs.h"

#define STR_EQUALS(x, y) if(strcmp(x, y) == 0)
#define STR_ASSIGN(field, string) strcpy(field, string)

struct uci_context *context = NULL;
struct uci_package *package = NULL;

static int start_uci_ctx(char *config){
    int rc = 0;
    syslog(LOG_INFO, "Starting the uci context");

    if ((rc = uci_load(context, config, &package)) != UCI_OK)
    {
        char *err; uci_perror(context, err);
        syslog(LOG_ERR, "Failed to start uci context, rc=%s", err); 
        uci_free_context(context);
        return rc;
    }

    return rc;
}

static void close_uci_ctx(){
    syslog(LOG_INFO, "Closing/Cleaning uci context");
    uci_free_context(context);
}

static int assign_event_value(struct event *event, struct uci_option *option, char *option_name){
    STR_EQUALS("topic", option_name) STR_ASSIGN((*event).topic, option->v.string);
    else STR_EQUALS("paramKey", option_name) STR_ASSIGN((*event).param_key, option->v.string);
    else STR_EQUALS("value", option_name) STR_ASSIGN((*event).value, option->v.string);
    else STR_EQUALS("compare", option_name) STR_ASSIGN((*event).compare, option->v.string);
    else STR_EQUALS("sender", option_name) STR_ASSIGN((*event).sender, option->v.string);
    else STR_EQUALS("senderPassw", option_name) STR_ASSIGN((*event).sender_passw, option->v.string);
    else STR_EQUALS("emailMsg", option_name) STR_ASSIGN((*event).email_msg, option->v.string);
    else STR_EQUALS("emailSubject", option_name) STR_ASSIGN((*event).email_subject, option->v.string);
    else STR_EQUALS("dataType", option_name){
        STR_EQUALS("number", option->v.string) (*event).dt = NUMBER;
        else {
            (*event).dt = STRING;
        }
    }
    else STR_EQUALS("recipient", option_name){
        struct uci_element *el;
        uci_foreach_element(&option->v.list, el){
            str_node *el_tmp = malloc(sizeof(str_node));

            el_tmp->next = NULL;
            el_tmp->obj = malloc(sizeof(char) * (strlen(el->name) + 1));

            strcpy(el_tmp->obj, el->name);
            list_addback_str(&event->recp_list, el_tmp);
        }
    }
    else syslog(LOG_WARNING, "A non existant option was parsed: %s", option_name);
}

int get_events(ev_node **events){
    int rc = 0;

    syslog(LOG_INFO, "Processing %s file's sections and options", TOPIC_CFG);
    struct uci_element *i, *j;

    uci_foreach_element(&package->sections, i)
    {
        /* temporary node and event struct, used to store iterator data */
        ev_node *ntmp = malloc(sizeof(ev_node));
        struct event *tmp = malloc(sizeof(struct event));
        tmp->recp_list = NULL;

        struct uci_section *section = uci_to_section(i);
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;

            assign_event_value(tmp, option, option_name);
        }
        ntmp->next = NULL;
        ntmp->obj = tmp;

        list_addback_ev(events, ntmp);
    }

    return rc;
}

/*
 * TODO: add a rc to this
 * add error checking
 */
int assign_topic_value(struct topic *topic, struct uci_option *option, char *option_name){
    STR_EQUALS("name", option_name) STR_ASSIGN((*topic).name, option->v.string);
    else STR_EQUALS("qos", option_name){
        (*topic).qos = atoi(option->v.string);
    }
}

int get_topics(tp_node **topics){
    int rc = 0;

    syslog(LOG_INFO, "Processing %s file's sections and options", TOPIC_CFG);
    struct uci_element *i, *j;
    uci_foreach_element(&package->sections, i)
    {
        tp_node *ntmp = malloc(sizeof(tp_node));
        struct topic *tmp = malloc(sizeof(struct topic));

        struct uci_section *section = uci_to_section(i);
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;
            syslog(LOG_DEBUG, "Got option name and value: %s %s", option_name, option->v.string);

            assign_topic_value(tmp, option, option_name);
            tmp->events = NULL;
        }   

        ntmp->next = NULL;
        ntmp->obj = tmp;

        list_addback_tp(topics, ntmp);
    }

    return rc;
}

int assign_ev_to_tp(tp_node **topics, ev_node *events){
    for(tp_node *tp_iter = *topics; tp_iter != NULL; tp_iter = tp_iter->next){
        char *tp_name = tp_iter->obj->name;

        /* TODO: this may cause leaks, as unassigned nodes don't get freed */ 
        for(ev_node *ev_iter = events; ev_iter != NULL; ev_iter = ev_iter->next){
            if(strcmp(tp_name, ev_iter->obj->topic) == 0){
                /* TODO: check whether this actually works;
                   maybe also add deleting, so that you don't
                   iterate through the same elements twice */
                list_addback_ev(&(tp_iter->obj->events), ev_iter);
            }
        }
    }
}

int load_topics(struct tp_node **head){
    context = uci_alloc_context();
    start_uci_ctx(TOPIC_CFG);

    get_topics(head);

    start_uci_ctx(EVENT_CFG);
    ev_node *events = NULL;
    get_events(&events);
    close_uci_ctx();

    assign_ev_to_tp(head, events);
}