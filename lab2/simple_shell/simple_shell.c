/*****************************************************************
* Matt Mallett
* CprE 308
* simple_shell.c
* Basic shell that spawns a new process from a list of programs.
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LS "/bin/ls"
#define DATE "/bin/date"
#define PS "/bin/ps"
#define FIREFOX "/usr/bin/firefox"

int main(){
	//pretty header with instructions
	int i;
	for(i = 0; i < 20; i++) printf("*");
	printf("\n");
	printf("* Simple Shell\n* Enter a program from this list\n");
	printf("* ls\n* date\n* ps\n* firefox\n");
	for(i = 0; i < 20; i++) printf("*");
	printf("\n");

	//get input
	char input[10];
	scanf("%s", input);

	//spawning
	int ret = fork();
	if(ret == 0){
		char * to_exec;
		if(strcmp(input, "ls") == 0) 
            to_exec = LS;
		else if(strcmp(input, "date") == 0)
			to_exec = DATE;
		else if(strcmp(input, "ps") == 0)
			to_exec = PS;
		else if(strcmp(input, "firefox") == 0)
			to_exec = FIREFOX;
		else{
			printf("Input not recognized\nExiting...\n");
			exit(0);
        }
        printf("Executing %s\n", input);
		execl(to_exec,input,0);
	}
}
	
