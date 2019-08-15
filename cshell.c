#include "include/cshell.h"
#include "include/embedded.h"
#include "include/signals.h"
#include "fcntl.h"

#define BUFSIZE 	256
#define HISTORY_COUNT	100



/* global variables/arrays */
char user[BUFSIZE];
char host[BUFSIZE];
char homedir[BUFSIZE];

char* history[HISTORY_COUNT];
int _history_current = 0;



/*	Small functions 	*/

int schell_number_of_arguments(char **cmd)
{
	int i = 0;
	while (cmd[i] != NULL)
	{
		i++;
	}
	return i;
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

void cshell_add_in_history(char *line)
{
	if ( history[_history_current % HISTORY_COUNT] )
	{
		free(history[_history_current % HISTORY_COUNT]);
	}
	history[(_history_current % HISTORY_COUNT)] = (char*)malloc(sizeof(char*) * strlen(line));
	strcpy(history[(_history_current % HISTORY_COUNT)], line);
	_history_current++;
}

void cshell_invoke()
{
	getcwd(homedir, BUFSIZE);
	printf("%s@%s:%s", user, host, homedir);
}


/* Build-in functions */

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

int cshell_history(char **args)
{
	int border;
	if (args[1] != NULL)
	{
		border = atoi(args[1]);
		if (border > HISTORY_COUNT || border <= 0)
			border = HISTORY_COUNT;
	}
	else border = HISTORY_COUNT;


	int cnt = 0;
	int i = (_history_current + (HISTORY_COUNT - border)) % HISTORY_COUNT;


	int hist_num;	// number in history
	if (_history_current <= HISTORY_COUNT)
	{	
		if (_history_current <= border)
		{
			hist_num = 1;
		}
		else
		{
			hist_num = _history_current - border + 1;
		}
	}
	else
	{
		hist_num = _history_current - HISTORY_COUNT + 1;
	}	
	do 
	{
		cnt++;
		if (history[i])
		{
			printf("%4d	%s\n", hist_num, history[i]);
			hist_num++;
		}
		i = ( i+1 ) % HISTORY_COUNT;
	} while ( i != _history_current % HISTORY_COUNT );
	return CSHELL_HISTORY_EXECUTE;
}


int cshell_freehistory(char **args)
{
	for (int i = 0; i < HISTORY_COUNT; i++)
	{
		if (history[i])
		{
			free(history[i]);
			history[i] = NULL;
		}
	}
	_history_current = 0;
	return CSHELL_FREEHISTORY_EXECUTE;
}



/* 	Parsers + performers 	*/


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

	/* adding the command in the history */
	cshell_add_in_history(line);

	return line;
}



char **cshell_tokenize_line(char *input_str);


int cshell_redirection_in_exec(char* line)
{
	char*  sline = strtok(line, ">");
	char*  file = strtok(NULL, "\n");
	file = strtok(file, "\n ");

	char** cmd = cshell_tokenize_line(sline);

	int oldstdout = dup(STDOUT); 	// duplicate old descriptor
	int out = creat(file, 0644); 	// create a new file
	dup2(out, STDOUT);	 	// redirection
	close(out);			// close file
	
	int res = cshell_launch(cmd);

	dup2(oldstdout, STDOUT);	// recovery stream
	close(oldstdout);

	if (res == CSHELL_EXEC_CMD || res == CSHELL_HISTORY_EXECUTE || res == CSHELL_FREEHISTORY_EXECUTE)
	{
		return CSHELL_REDIRECTED_EXEC_CMD;
	}
	else
	{
		return CSHELL_REDIRECTED_FAIL_EXEC_CMD;
	}

}

int cshell_redirection_out_exec(char* line)
{
	char*  sline = strtok(line, "<");
	char*  file = strtok(NULL, "\n");
	file = strtok(file, "\n ");

	char** cmd = cshell_tokenize_line(sline);

	int oldstdout = dup(0); 		// duplicate old descriptor
	int out = open(file, O_RDONLY, 0); 	// create a new file
	dup2(out, 0);		 		// redirection
						// close file
	int res = cshell_launch(cmd);

	dup2(oldstdout, 0);	// recovery stream
	close(oldstdout);

	if (res == CSHELL_EXEC_CMD || res == CSHELL_HISTORY_EXECUTE || res == CSHELL_FREEHISTORY_EXECUTE)
	{
		return CSHELL_REDIRECTED_EXEC_CMD;
	}
	else
	{
		return CSHELL_REDIRECTED_FAIL_EXEC_CMD;
	}
}




int cshell_redirection_in_detected(char* line)
{
	if (!cshell_detect_char_in_line(line, '>'))
	{
		return 0;
	}
	return 1;
}

int cshell_redirection_out_detected(char* line)
{
	if (!cshell_detect_char_in_line(line, '<'))
	{
		return 0;
	}
	return 1;
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
            				dup2(fd[STDOUT], STDOUT);
				}
				close(fd[0]);

				if (cshell_redirection_in_detected(cmd[i]))
				{
					if (cshell_redirection_in_exec(cmd[i]) == CSHELL_REDIRECTED_EXEC_CMD)
						exit(EXIT_SUCCESS);
					exit (EXIT_FAILURE);
				}
				
				if (cshell_redirection_out_detected(cmd[i]))
				{
					if (cshell_redirection_out_exec(cmd[i]) == CSHELL_REDIRECTED_EXEC_CMD)
						exit(EXIT_SUCCESS);
					exit (EXIT_FAILURE);
				}


				char** exec_cmd = cshell_tokenize_line(cmd[i]);

				if ( cshell_launch(exec_cmd) == CSHELL_EXEC_CMD)
					exit(EXIT_SUCCESS);

				exit (EXIT_FAILURE);
        		}
      			else
        		{
       	 	 		wait(NULL);
          			close(fd[STDOUT]);
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
	signal(SIGINT, sigint_handler);

	char *line = NULL;
	char **tokenizingline = NULL;
	char **cmd_exec = (char**) malloc((long)CSHELL_MAX_PIPE_NUM * sizeof(char*));
	
	int status;
	
	while (1)
	{
		line = cshell_read_line();

		/* */
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
			if (cshell_redirection_in_detected(line))
			{
				cshell_redirection_in_exec(line);
				continue;
			}
			if (cshell_redirection_out_detected(line))
			{
				cshell_redirection_out_exec(line);
				continue;
			}
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
	
