#ifndef EMBEDDED_H
#define EMBEDDED_H


// Embedded functions
int cshell_exit(char**);
int cshell_cd(char**);
int cshell_history(char**);
int cshell_freehistory(char**);
int cshell_dirs(char**);
int cshell_pushd(char**);
int cshell_popd(char**);


// Context
char *embedded_str[] = {
	"exit",
	"cd",
	"history",
	"freehistory",
	"dirs",
	"pushd",
	"popd"
};


// Function array
int (*embedded_func[])(char **) = {
	&cshell_exit,
	&cshell_cd,
	&cshell_history,
	&cshell_freehistory,
	&cshell_dirs,
	&cshell_pushd,
	&cshell_popd
};

#define CSHELL_NUM_EMBEDDED	 	sizeof(embedded_func)/sizeof(char*)

#endif
