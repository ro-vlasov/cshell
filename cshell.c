
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#define NARGS 5


// Embedded functions
int cshell_exit(char**);

// Context
char *embedded_str[] = {
	"exit"
};


// Function array
int (*embedded_func[])(char **) = {
	&cshell_exit
};

#define CSHELL_NUM_EMBEDDED 	sizeof(embedded_func)/sizeof(char*)
#define CSHELL_EXEC_CMD		-1




int cshell_exit(char **args)
{
	return 0;
}





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


int cshell_launch(char **cmd)
{
	for (int i = 0; i < CSHELL_NUM_EMBEDDED; i++)
	{
		if (strcmp(cmd[0], embedded_str[i]) == 0)
			return (*embedded_func[i])(cmd);
	}
	return cshell_exec_command(cmd);
}


int cshell_exec_command(char **cmd)
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
			return CSHELL_EXEC_CMD;
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
	char *line;
	char **tokenizingline;
	
	while (1)
	{
		line = cshell_read_line();
		tokenizingline = cshell_tokenize_line(line);
		if (cshell_launch(tokenizingline) == 0)
		{
			return 0;
		}
	}
}
	
