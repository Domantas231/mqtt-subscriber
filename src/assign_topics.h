#ifndef ASSIGN_TOPICS_H
#define ASSIGN_TOPICS_H

int assign_topic_value(struct topic *topic, struct uci_option *option, char *option_name);
int assign_event_value(struct event *event, struct uci_option *option, char *option_name);
int assign_ev_to_tp(struct tp_node **topics, struct ev_node *events);

#endif