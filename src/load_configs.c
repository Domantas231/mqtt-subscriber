#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <uci.h>

#include "load_configs.h"
#include "linked_list.h"

struct uci_context *context = NULL;
struct uci_package *package = NULL;

/*
 * TODO: Check whether uci_alloc_context doesn't leak
 * when trying to call it 2 times before calling
 * uci_free_context
 */
int parse_config_data(node **events, node **topics){
    int rc = 0;
    context = uci_alloc_context();

    get_events(events);
    get_topics(topics);

    close_uci_ctx();
}

static int start_uci_ctx(char *config){
    int rc = 0;
    syslog(LOG_INFO, "Starting the uci context");

    if ((rc = uci_load(context, config, &package)) != UCI_OK)
    {
        syslog(LOG_ERR, "Failed to start uci context, rc=%d", rc);
        uci_free_context(context);
        return rc;
    }

    return rc;
}

static void close_uci_ctx(){
    syslog(LOG_INFO, "Closing/Cleaning uci context");
    uci_free_context(context);
}

/*
 * TODO: add a rc to this
 * Add error checking
 */
static int assign_event_value(struct event *event, struct uci_option *option, char *option_name){
    if(strcmp(option_name, "topic") == 0){
        strcpy((*event).topic, option->v.string);
    }
    else if(strcmp(option_name, "dataType") == 0){
        if(strcmp(option->v.string, "number")){
            (*event).dt = NUMBER;
        } else {
            (*event).dt = STRING;
        }
    }
    else if(strcmp(option_name, "paramKey") == 0){
        strcpy((*event).param_key, option->v.string);
    }
    else if(strcmp(option_name, "value") == 0){
        strcpy((*event).value, option->v.string);
    }
    else if(strcmp(option_name, "compare") == 0){
        strcpy((*event).compare, option->v.string);
    }
    else if(strcmp(option_name, "recipient") == 0){
        struct uci_element *el;
        uci_foreach_element(&option->v.list, el){
            node *el_tmp = malloc(sizeof(node));

            el_tmp->next = NULL;
            el_tmp->obj = malloc(sizeof(char) * (strlen(el->name) + 1));

            strcpy(el_tmp->obj, el->name);
            list_addback(&event->recp_list, el_tmp);
        }
    }
    else if(strcmp(option_name, "sender") == 0){
        strcpy((*event).sender, option->v.string);
    }
    else if(strcmp(option_name, "senderPassw") == 0){
        strcpy((*event).sender_passw, option->v.string);
    }
    else{
        syslog(LOG_WARNING, "A non existant option was parsed: %s", option_name);
    }
}

int get_events(node **events){
    int rc = 0;

    if((rc = start_uci_ctx(TOPIC_CFG))) return rc;

    syslog(LOG_INFO, "Processing %s file's sections and options", TOPIC_CFG);
    struct uci_element *i, *j;

    uci_foreach_element(&package->sections, i)
    {
        /* temporary node and event struct, used to store iterator data */
        node *ntmp = malloc(sizeof(node));
        struct event *tmp = malloc(sizeof(struct event));
        tmp->recp_list = NULL;

        struct uci_section *section = uci_to_section(i);
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;

            /* This might be a bit over-engineered */
            if(option->type == UCI_TYPE_STRING)
                syslog(LOG_DEBUG, "Got option (string type) name and value: %s %s", option_name, option->v.string);

            // else if(option->type == UCI_TYPE_LIST)
            //     syslog(LOG_DEBUG, "Got option (list type) name and value: %s %s", option_name, option->v.list);

            assign_event_value(tmp, option, option_name);
        }
        ntmp->next = NULL;
        ntmp->obj = tmp;

        list_addback(events, ntmp);
    }

    return rc;
}

/*
 * TODO: add a rc to this
 * add error checking
 */
int assign_topic_value(struct topic *topic, struct uci_option *option, char *option_name){
    if(strcmp(option_name, "name") == 0){
        (*topic).name = option->v.string;
    }
    else if(strcmp(option_name, "qos") == 0){
        (*topic).qos = atoi(option->v.string);
    }
}

int get_topics(node **topics){
    int rc = 0;

    if ((rc = start_uci_ctx(TOPIC_CFG))) return rc;

    syslog(LOG_INFO, "Processing %s file's sections and options", TOPIC_CFG);
    struct uci_element *i, *j;
    uci_foreach_element(&package->sections, i)
    {
        node *ntmp = malloc(sizeof(node));
        struct topic *tmp = malloc(sizeof(struct topic));

        struct uci_section *section = uci_to_section(i);
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;
            syslog(LOG_DEBUG, "Got option name and value: %s %s", option_name, option->v.string);

            assign_topic_value(tmp, option, option_name);
        }   

        ntmp->next = NULL;
        ntmp->obj = tmp;

        list_addback(topics, ntmp);
    }

    return rc;
}