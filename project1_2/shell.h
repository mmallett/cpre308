/****************************************************************************
* Matt Mallett
* CprE 308
* shell.c
* Unix Shell
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
extract type from arg0
-1 error
0 pass to exec
1 cd
2 cwd
3 pid
4 ppid
5 history
6 quit
*/
int get_type(char * arg0);

/*
determine if & is last argument
and if command should wait until
child finishes
*/
int get_block(char * arg);

/*
initializes prompt to 308sh
*/
void init_prompt(char * prompt);

/*
change prompt to new_prompt
call after init_prompt
*/
void change_prompt(char * prompt, char * new_prompt);

/*
prints the prompt
*/
void print_prompt(char * prompt);

/*
prints standardized output format
for shell output
*/
void print_out(char * output);

/*
get a string containing the exit status of a child
*/
void get_status_str(int status, int pid, char * buffer);

/*
executes a command based on input
return:
1 if exit command was given
0 else
*/
int execute(int cmd_type, int cmd_block, char * filename, char *argv[]);
