#include "shell.h"

int main()
{
	do
	{
		char in[80];
		printf("Enter string");
		scanf("%s",in);
		command_t cmd = parse(in);
	}
}
