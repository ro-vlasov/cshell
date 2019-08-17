#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define STACK_COUNT	100

int position = 0;

void _stack_push(char** stack,const char* element)
{
	if (position < STACK_COUNT)
	{
		stack[position] = element;
		position++;
		return;
	}
	else
	{
		printf("\nSTACK DIRECTORIES OVERFLOW\n");
		return;
	}
}


char* _stack_pop(char** stack)
{
	if (position == 0)
	{
		printf("\nSTACK DIRECTORIES IS CLEAR\n");
		return NULL;
	}
	if (position == 1)
	{
		return stack[0];
	}
	if (position == 2)
	{
		position = 1;
		free(stack[position]);	// because getcwd(NULL,0) uses malloc
		return stack[0];
	}
	position -= 1;
	free(stack[position]);		// because getcwd(NULL,0) uses malloc
	return stack[position - 1];	
}
	

void _stack_change_top(char** stack, const char* element)
{
	stack[position - 1] = element;
}

const char* _stack_peek(char** stack)
{
	return stack[position - 1];
}
