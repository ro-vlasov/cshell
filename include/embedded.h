#ifndef EMBEDDED_H
#define EMBEDDED_H


// Embedded functions
int cshell_exit(char**);
int cshell_cd(char**);
// Context
char *embedded_str[] = {
	"exit",
	"cd"
};


// Function array
int (*embedded_func[])(char **) = {
	&cshell_exit,
	&cshell_cd
};

#define CSHELL_NUM_EMBEDDED	 	sizeof(embedded_func)/sizeof(char*)

#endif
