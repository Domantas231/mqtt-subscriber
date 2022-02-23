#include <string.h>
#include <stdlib.h>

#include "compare_msg.h"
#include "linked_list.h"

/* 
 * TODO: change atoi() to strtol or strtod
 */

#define INT_COMPARE(field, cmp_type, msg_value, tp_value, res)              \
            if(strcmp(field, #cmp_type) == 0){                              \
                if(atoi(value) cmp_type atoi(ev.value)) *res = 0;           \
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

/*
 * TODO: dont need a res variable, because this function can't break, maybe
 */
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