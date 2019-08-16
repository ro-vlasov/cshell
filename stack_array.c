#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define STACK_COUNT	100

int position = 0;

void _stack_push(char** stack, char* element)
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


void _stack_pop(char** stack, char** dir)
{
	if (position == 0)
	{
		printf("\nSTACK DIRECTORIES IS CLEAR\n");
	}
	if (position == 1)
	{
		strcpy(*dir, stack[0]);

	}
	position--;

	*dir = malloc(strlen(stack[position - 1]));
	strcpy(*dir, stack[position - 1]);
}
	



void _stack_change_top(char** stack, char* element)
{
	stack[position - 1] = element;
}

