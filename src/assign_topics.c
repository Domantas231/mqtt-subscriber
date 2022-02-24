#include <stdlib.h>
#include <syslog.h>
#include <uci.h>
#include <string.h>

#include "load_topics.h"
#include "linked_list.h"

#define STR_EQUALS(x, y) if(strcmp(x, y) == 0)
#define STR_ASSIGN(field, string) strcpy(field, string)

/* ============================
 * FINDING AND ASSIGNING VALUES
 * ============================ */

int assign_topic_value(struct topic *topic, struct uci_option *option, char *option_name){
    STR_EQUALS("name", option_name) STR_ASSIGN((*topic).name, option->v.string);
    else STR_EQUALS("qos", option_name){
        (*topic).qos = strtol(option->v.string, NULL, 10);
    }
}

int assign_event_value(struct event *event, struct uci_option *option, char *option_name){
    STR_EQUALS("topic", option_name) STR_ASSIGN((*event).topic, option->v.string);
    else STR_EQUALS("paramKey", option_name) STR_ASSIGN((*event).param_key, option->v.string);
    else STR_EQUALS("value", option_name) STR_ASSIGN((*event).value, option->v.string);
    else STR_EQUALS("compare", option_name) STR_ASSIGN((*event).compare, option->v.string);
    else STR_EQUALS("sender", option_name) STR_ASSIGN((*event).sender, option->v.string);
    else STR_EQUALS("senderPassw", option_name) STR_ASSIGN((*event).sender_passw, option->v.string);
    else STR_EQUALS("emailMsg", option_name) STR_ASSIGN((*event).email_msg, option->v.string);
    else STR_EQUALS("emailSubject", option_name) STR_ASSIGN((*event).email_subject, option->v.string);
    else STR_EQUALS("smtpServer", option_name) STR_ASSIGN((*event).smtp_server, option->v.string);
    else STR_EQUALS("smtpServer", option_name) STR_ASSIGN((*event).smtp_server, option->v.string);
    else STR_EQUALS("smtpPort", option_name) STR_ASSIGN((*event).smtp_port, option->v.string);
    else STR_EQUALS("useSSL", option_name) STR_ASSIGN((*event).use_ssl, option->v.string);
    else STR_EQUALS("caFileLoc", option_name) STR_ASSIGN((*event).ca_path, option->v.string);
    else STR_EQUALS("dataType", option_name){
        STR_EQUALS("number", option->v.string) (*event).dt = NUMBER;
        else (*event).dt = STRING;
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

/* Add event nodes to their topics */
int assign_ev_to_tp(tp_node **topics, ev_node *events){
    for(tp_node *tp_iter = *topics; tp_iter != NULL; tp_iter = tp_iter->next){
        char *tp_name = tp_iter->obj->name;

        for(ev_node *ev_iter = events; ev_iter != NULL; ev_iter = ev_iter->next){
            if(strcmp(tp_name, ev_iter->obj->topic) == 0)
            {
                struct ev_node *ev_tmp = malloc(sizeof(struct ev_node));
                struct event *tmp = malloc(sizeof(struct event));

                *tmp = *ev_iter->obj;
                ev_tmp->obj = tmp;
                ev_tmp->next = NULL;

                list_addback_ev(&(tp_iter->obj->events), ev_tmp);
            }
        }
    }
}