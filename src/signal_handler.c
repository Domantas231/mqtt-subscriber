#include <stdio.h>
#include <memory.h>
#include <mosquitto.h>

#include "signal_handler.h"
#include "email_handler.h"

struct sigaction action;

struct mosquitto *mosq;

void sig_handler(int signo) {
    mosquitto_disconnect(mosq);
}

int setup_signals(struct mosquitto *m){
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sig_handler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    mosq = m;
}