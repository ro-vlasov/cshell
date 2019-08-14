#include "signal.h"
#include <stdio.h>

typedef void (*sighandler_t)(int);

void handle_signal(int signo)
{
	fflush(stdout);
}

