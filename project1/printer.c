#include "shell.h"

char prompt[100];

void init_prompt()
{
	strcpy(prompt,"308sh");
}

void change_prompt(char * new_prompt)
{
	strcpy(prompt, new_prompt);
}

void print_prompt()
{
	printf("\n%s>",prompt);
}

void print_out(char * output)
{
	printf("\n>>>%s\n",output);
}
