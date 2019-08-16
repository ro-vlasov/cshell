#ifndef CSHELL_H
#define CSHELL_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define CSHELL_MAX_PIPE_NUM 		10
#define NARGS 				5
#define STDOUT				1



#define CSHELL_CD_ERROR			-10
#define CSHELL_FORK_ERROR		-11
#define CSHELL_EXEC_PIPE_FAIL		-20
#define CSHELL_REDIRECTED_FAIL_EXEC_CMD	-30
#define CSHELL_PUSHD_ERROR		-50
#define CSHELL_POPD_ERROR		-60

#define CSHELL_EXEC_CMD			10
#define CSHELL_EXIT_PIPE_SUCCESS 	20
#define CSHELL_REDIRECTED_EXEC_CMD	30


#define CSHELL_EXIT			0
#define CSHELL_CD_EXECUTE		1
#define CSHELL_HISTORY_EXECUTE		2
#define CSHELL_FREEHISTORY_EXECUTE	3
#define CSHELL_DIRS			4
#define CSHELL_PUSHD			5
#define CSHELL_POPD			6


#endif
