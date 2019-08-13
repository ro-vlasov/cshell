#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


#define NARGS 5


char **cshell_tokenize_line(char *input_str)
{
	char **cmd = (char **)malloc(NARGS*sizeof(char*));
	char *parsed;
	char *separator = " ";
	int i = 0;

	
	parsed = strtok(input_str, separator);

	if (parsed == NULL)
	{
		cmd[i++] = input_str;
		cmd[i] = NULL;
		return cmd;
	}

	while (parsed != NULL)
	{
		if (!(strcmp(parsed, "\n")))
			break;

		if (i <= NARGS)
		{
			cmd[i++] = parsed;
			parsed = strtok(NULL, separator);
		}
		else
		{
			printf("\n a lot of args");
			return NULL;
		}
	}

	cmd[i] = NULL;
	return cmd;
}


char * cshell_read_line()
{
	printf("%s", "cshell$> ");
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	line[strlen(line) - 1] = '\0';
	return line;
}


void cshell_exec_command(char **cmd)
{
	pid_t _pid = fork();
	int* status;
	if (_pid == 0)
	{
		if (execvp(cmd[0], cmd) == -1)
		{
			printf("%s:%d", "errno:", errno);
		}
		exit (EXIT_FAILURE);
	}
	else
	{
		if (_pid > 0)
		{
			wait(status);
		}
		else
		{
			printf("Fork error");
			exit (EXIT_FAILURE);
		}
	}
}

int main()
{
	char *a = cshell_read_line();
	char **b = cshell_tokenize_line(a);
	int _pidfork = fork();
	int *status;
	if (_pidfork == 0)
	{
		cshell_exec_command(b);
		exit(0);
	}
	else
	{
		wait(status);
	}
	//cshell_exec_command(b);	
}
	
