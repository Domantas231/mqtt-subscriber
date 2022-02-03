#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <uci.h>

#include "linked_list.h"
#include "load_events.h"

/*
 * The topic linked list needs to be freed
 * by the function user!!!
 */
int get_events(node **events){
    int rc = 0;

    /*
     * Initialise uci context, which is needed 
     * to get config info from file
     */
    syslog(LOG_INFO, "Starting the uci context");
    struct uci_context *context = uci_alloc_context();
    struct uci_package *package;
    if ((rc = uci_load(context, CFG_NAME, &package)) != UCI_OK)
    {
        syslog(LOG_ERR, "Failed to start uci context, rc=%d", rc);
        uci_free_context(context);
        return rc;
    }

    syslog(LOG_INFO, "Processing config file's sections and options");
    struct uci_element *i, *j;
    /*
     * Loop through each section in the file
     */
    uci_foreach_element(&package->sections, i)
    {
        /*
         * Allocate space for a new topic
         * which will be added to the linked list
         */
        node *ntmp = malloc(sizeof(node));
        struct event *tmp = malloc(sizeof(struct event));

        /*
         * Loop through each element/option
         * in a section
         */
        struct uci_section *section = uci_to_section(i);
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;
            syslog(LOG_DEBUG, "Got option name and value: %s %s", option_name, option->v.string);

            /*
             * Assign values to the allocated topic struct
             * TODO: This can be probably implemented in a better way
             */
            if(strcmp(option_name, "topic") == 0){
                strcpy(tmp->topic, option->v.string);
            }
            else if(strcmp(option_name, "dataType") == 0){
                if(strcmp(option->v.string, "number")){
                    tmp->dt = NUMBER;
                } else {
                    tmp->dt = STRING;
                }
            }
            else if(strcmp(option_name, "paramKey")){
                strcpy(tmp->param_key, option->v.string);
            }
            else if(strcmp(option_name, "value")){
                strcpy(tmp->value, option->v.string);
            }
            else if(strcmp(option_name, "compare")){
                strcpy(tmp->compare, option->v.string);
            }
            else if(strcmp(option_name, "recipient")){
                /* TODO: temp only one no headache thank */
                strcpy(tmp->recipient, option->v.string);
            }
            else if(strcmp(option_name, "sender")){
                strcpy(tmp->recipient, option->v.string);
            }
            else{
                syslog(LOG_WARNING, "A non existant option was parsed: %s", option_name);
            }
        }

        ntmp->next = NULL;
        ntmp->obj = tmp;

        /*
         * After going through all of the options
         * in a section, add them to the list
         */
        list_addback(events, ntmp);
    }

    syslog(LOG_INFO, "Closing/Cleaning uci context");
    uci_free_context(context);
    return rc;
}