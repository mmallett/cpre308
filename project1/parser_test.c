#include "shell.h"

int main(){

	init_prompt();
	//print_prompt();
	//change_prompt("I Hate C");
	//print_out("Prompt Changed to I Hate C");
	//print_prompt();
	
	//going to recycle this guy
	command_t * mycmd = alloc_command_t();
	//print_prompt();
	//char buffer[40];
	//scanf("%s",buffer);
	parse("Hello world",mycmd);
    /*int i;
    for(i = 0; i < MAX_ARG_COUNT; i++)
    {
        if(mycmd -> argv[i] == NULL) break;
        print_out(mycmd -> argv[i]);
    }*/
	

	/*
	char buffer[40];
	fputs("Enter Command: ", stdout);
	fflush(stdout);
	fgets(buffer, sizeof buffer, stdin);
	printf("input text = %s\n", buffer);
	printf("About to enter parse()");
	command_t c = parse(buffer);
	printf("%s",c.path);
	*/
	return 0;
}
