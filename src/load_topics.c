#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <uci.h>

#include "load_topics.h"
#include "linked_list.h"
#include "assign_topics.h"

struct uci_context *context = NULL;
struct uci_package *package = NULL;

/* ====================
 * UCI RELATED HANDLING
 * ==================== */

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

/* =================
 * PARSING FUNCTIONS
 * ================= */

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

int get_events(ev_node **events){
    int rc = 0;

    syslog(LOG_INFO, "Processing %s file's sections and options", EVENT_CFG);
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

            syslog(LOG_DEBUG, "Got option name and value: %s %s", option_name, option->v.string);
            assign_event_value(tmp, option, option_name);
        }
        ntmp->next = NULL;
        ntmp->obj = tmp;

        list_addback_ev(events, ntmp);
    }

    return rc;
}

/* =============
 * MAIN FUNCTION
 * ============= */

int load_topics(struct tp_node **head, struct ev_node **events){
    int rc = 0;
    context = uci_alloc_context();

    if((rc = start_uci_ctx(TOPIC_CFG)) != 0) return rc;
    get_topics(head);

    if((rc = start_uci_ctx(EVENT_CFG)) != 0) return rc;
    get_events(events);
    
    close_uci_ctx();

    assign_ev_to_tp(head, *events);

    return rc;
}