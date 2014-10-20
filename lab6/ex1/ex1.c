#include <signal.h>
#include <stdio.h>

void my_routine();

main()
{
	signal(SIGQUIT, my_routine);
	printf("Entering infinite loop\n");
	while(1) { sleep(10); }
	printf("can't get here\n");
}

void my_routine()
{
	printf("Running my_routine\n");
}
