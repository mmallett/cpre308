#include <stdio.h>
#include <stdlib.h>

/********************************************
COMMAND_T SPECIFICATION
********************************************/

enum CMD
{
	custom,
	cd,
	cwd,
	quit,
	history,
	pid,
	ppid,
	error
};

typedef struct
{
	enum CMD type;
	char ** argv;
	int argc;
	int block;
}command_t;
	

/*********************************************
PARSER.C PROTOTYPES
*********************************************/

#include <string.h>

#define MAX_INPUT_SIZE 100
#define MAX_ARG_COUNT 15

//parse input text and construct command_t out of it
void parse(char * input, command_t * mycmd);

//allocates memory for command_t data structure
command_t* alloc_command_t();

//frees memory associated with command_t data structure
void free_command_t(command_t * to_free);

//check if character is contained in string
int str_contains(char * s, char c);

//returns substring from start to end in string, no error checking
void substring(char * source, char * substring, int start, int end);

/*********************************************
PRINTER.C PROTOTYPES
*********************************************/

//set maximum prompt length
#define MAX_PROMPT_LENGTH 100

//init prompt, must be called before prompt can be used
void init_prompt();

//change default prompt
void change_prompt(char * new_prompt);

//print prompt for input
void print_prompt();

//standardized output format
void print_out(char * output);
