#include "include/signals.h"
#include "include/cshell.h"
#include <stdio.h>

void sigint_handler(int sig)
{
	printf("\nPlease write \"exit\" to exit\n");
	cshell_invoke();
	printf("%s", " cshell$> ");

	fflush(stdout);
	return;
}
