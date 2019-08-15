#ifndef EMBEDDED_H
#define EMBEDDED_H


// Embedded functions
int cshell_exit(char**);
int cshell_cd(char**);
int cshell_history(char**);

// Context
char *embedded_str[] = {
	"exit",
	"cd",
	"history"
};


// Function array
int (*embedded_func[])(char **) = {
	&cshell_exit,
	&cshell_cd,
	&cshell_history
};

#define CSHELL_NUM_EMBEDDED	 	sizeof(embedded_func)/sizeof(char*)

#endif
