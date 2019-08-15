#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>


/* function prototype */
void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);
void sigquit_handler(int sig);

typedef void handler_t(int);
handler_t* cshell_signal(int signum, handler_t *sighandler);


#endif
