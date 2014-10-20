#include <stdio.h>
#include <signal.h>

void my_routine();

main()
{
	signal(SIGINT, my_routine);
	signal(SIGQUIT, my_routine);
	printf("Entering infinite loop\n");
	while(1) { sleep(10); }
	printf("Can't get here\n");
}

void my_routine(int signo)
{
	printf("The signal number is %d\n", signo);
}
