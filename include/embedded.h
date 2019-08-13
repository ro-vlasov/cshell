#ifndef EMBEDDED_H
#define EMBEDDED_H



/* 	include/embedded.h 	*/



typedef int (*embedded)(char**);

// Embedded functions

embedded cshell_exit;


// Context
char *embedded_str[] = {
	"exit"
};


// Function array
embedded embedded_func[] = {
	&cshell_exit
};

#define CSHELL_NUM_EMBEDDED 	sizeof(embedded_func)/sizeof(char*)


#endif
