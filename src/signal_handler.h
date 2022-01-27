#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t daemonise;
extern struct sigaction action;

int setup_signals();

#endif