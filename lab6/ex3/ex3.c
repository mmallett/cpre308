#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void my_proc();

main()
{
	signal(SIGFPE, my_proc);
	int a = 1;
	a = a/0;
}

void my_proc()
{
	printf("Division by zero\n");
	exit(1);
}
