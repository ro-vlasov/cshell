
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





int __exit(char **args)
{
	return 0;
}



embedded cshell_exit = &__exit;



