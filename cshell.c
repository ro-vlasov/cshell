#include "include/cshell.h"
#include "include/embedded.h"

#define BUFSIZE 	256
char user[BUFSIZE];
char host[BUFSIZE];
char homedir[BUFSIZE];


void cshell_invoke()
{
	getcwd(homedir, BUFSIZE);
	printf("%s@%s:%s", user, host, homedir);
}


int cshell_exit(char **args)
{
	return CSHELL_EXIT;
}

int cshell_cd(char **args)
{
	if (args[1] == NULL)
	{
		if (chdir("/home/qfour") == -1)
		{
			printf("cd : can't change directory\n");
			return CSHELL_CD_ERROR;
		}
		return CSHELL_CD_EXECUTE;
	}
	else
	{
		if (args[2] == NULL)
		{
			if (chdir(args[1]) == -1)
			{
				printf("cd : can't change directory\n");
				return CSHELL_CD_ERROR;
			}
			return CSHELL_CD_EXECUTE;
		}
		else
		{
			printf("cd: bad arguments\n");
			return CSHELL_CD_ERROR;
		}
	}
}

char **cshell_tokenize_line(char *input_str);


int schell_number_of_arguments(char **cmd)
{
	int i = 0;
	while (cmd[i] != NULL)
	{
		i++;
	}
	return i;
}

int cshell_pipe_exec_cmd(char **cmd) 
{
  	int fd[2];
  	int fd_in = 0;
	int i = 0;
	int cnt_pipes = schell_number_of_arguments(cmd);

	pid_t pid;
	
	while (i < cnt_pipes)
	{
		pipe(fd);
		pid = fork();
		if (pid == -1)
        	{
          		exit(EXIT_FAILURE);
        	}
      		else
		{
			if (pid == 0)
        		{
          			dup2(fd_in, 0);	//change the input according to the old one 
          			if ((cmd[i+1]) != NULL)
				{
            				dup2(fd[1], 1);
				}
				close(fd[0]);
				char**a = cshell_tokenize_line(cmd[i]);
          			execvp(a[0], a);
				exit(EXIT_FAILURE);
        		}
      			else
        		{
       	 	 		wait(NULL);
          			close(fd[1]);
          			fd_in = fd[0]; //save the input for the next command
          			i++;
      			}
		}	
	}
  	return CSHELL_EXIT_PIPE_SUCCESS;
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
			printf("cmd: a lot of args\n");
			return NULL;
		}
	}

	cmd[i] = NULL;
	return cmd;
}

char * cshell_read_line()
{
	/* user, host, path */
	cshell_invoke();

	/* */
	printf("%s", " cshell$> ");
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
		{
			return (*embedded_func[i])(cmd);
		}
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
			printf("%s:%d\n", "execution error: errno = ", errno);
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
			printf("Fork error\n");
			exit (EXIT_FAILURE);
		}
	}
}


char cshell_detect_char_in_line(char *line, char ch)
{
	int i = 0;
	while (line[i] != '\0')
	{
		if (line[i] == ch)
			return 1;
		i++;
	}
	return 0;
}

int cshell_pipe_detected(char *line, char **cmd_exec)
{
	if (!cshell_detect_char_in_line(line, '|'))
	{
		return 0;
	}

	int pipe_cnt = 0;
	cmd_exec[pipe_cnt++] = strtok(line, "|");
	while ((cmd_exec[pipe_cnt] = strtok(NULL, "|")) != NULL)
	{
		pipe_cnt++;
	}
	cmd_exec[pipe_cnt] = NULL;
	return pipe_cnt;
}


int main()
{
	
	/* initial environment*/
	getlogin_r(user, BUFSIZE);
	gethostname(host, BUFSIZE);
	/* */

	char *line = NULL;
	char **tokenizingline = NULL;
	char **cmd_exec = (char**) malloc((long)CSHELL_MAX_PIPE_NUM * sizeof(char*));
	
	int status;
	
	while (1)
	{
		line = cshell_read_line();
		int pcnt = cshell_pipe_detected(line, cmd_exec);
		if (pcnt)
		{
			pid_t pid = fork();
			if (pid == 0)
			{
 				if (cshell_pipe_exec_cmd(cmd_exec) > 0)
				{
					return CSHELL_EXIT_PIPE_SUCCESS; 
				}
			}
			else
			{
				wait(&status);
			}
		}
		else
		{
			tokenizingline = cshell_tokenize_line(line);
			if (cshell_launch(tokenizingline) == 0)
			{
				if (tokenizingline)
					free(tokenizingline);
				if (cmd_exec)
					free(cmd_exec);
				return 0;
			}
		}
		free(tokenizingline);
		tokenizingline = NULL;
	}
}
	
