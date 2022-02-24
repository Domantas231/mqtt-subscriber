#include <string.h>
#include <stdlib.h>

#include "compare_msg.h"
#include "linked_list.h"

#define INT_COMPARE(field, cmp_type, msg_value, tp_value, res)              \
            if(strcmp(field, #cmp_type) == 0){                              \
                if(strtol(msg_value, NULL, 10) cmp_type strtol(ev.value, NULL, 10)) *res = 0;           \
                else *res = 1;                                              \
            }

#define STR_COMPARE(field, cmp_type, msg_value, tp_value, res)              \
            if(strcmp(field, #cmp_type) == 0){                              \
                if(strcmp(msg_value, tp_value) cmp_type 0) *res = 0;       \
                else *res = 1;                                              \
            }


int compare_str_msg(struct event ev, char *value, int *res){
    STR_COMPARE(ev.compare, ==, value, ev.value, res)
    else STR_COMPARE(ev.compare, !=, value, ev.value, res)

    return 0;
}

int compare_int_msg(struct event ev, char *value, int *res){
    INT_COMPARE(ev.compare, >, value, ev.value, res)
    INT_COMPARE(ev.compare, <, value, ev.value, res)
    INT_COMPARE(ev.compare, ==, value, ev.value, res)
    INT_COMPARE(ev.compare, !=, value, ev.value, res)
    INT_COMPARE(ev.compare, <=, value, ev.value, res)
    INT_COMPARE(ev.compare, >=, value, ev.value, res)
    else return 1;

    return 0;
}