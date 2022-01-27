#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <uci.h>

int get_topics(struct topic topics[], int t_num){
    int rc = 0;

    const char *config_name = CFG_NAME;

    /*
     * Initialise uci context, which is needed 
     * to get config info from file
     */
    syslog(LOG_INFO, "Starting the uci context");
    struct uci_context *context = uci_alloc_context();
    struct uci_package *package;
    if ((rc = uci_load(context, config_name, &package)) != UCI_OK)
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
        struct uci_section *section = uci_to_section(i);
        /*
         * Loop through each element/option
         * in a section
         */
        uci_foreach_element(&section->options, j)
        {
            struct uci_option *option = uci_to_option(j);
            char *option_name = option->e.name;
            syslog(LOG_DEBUG, "Got option name and value: %s %s", option_name, option->v.string);

            /*
             * Assign values to the topic struct
             */
            if(strcmp(option_name, "name") == 0){
                topics[t_num].name = option->v.string;
            }
            else if(strcmp(option_name, "qos") == 0){
                topics[t_num].qos = atoi(option->v.string);
            }
        }
        t_num++;
    }

    syslog(LOG_INFO, "Closing/Cleaning uci context");
    uci_free_context(context);
    return rc;
}