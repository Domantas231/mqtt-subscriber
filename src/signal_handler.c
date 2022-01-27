#include <stdio.h>
#include <memory.h>

#include "signal_handler.h"

volatile sig_atomic_t daemonise = 1;

struct sigaction action;

void sig_handler(int signo) {
    daemonise = 0;
}

int setup_signals(){
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sig_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}