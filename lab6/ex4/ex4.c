#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

char msg[100];

void my_alarm();

main(int argc, char * argv[])
{
	int time;
	if( argc < 3)
	{
		printf("not enough params\n");
		exit(1);
	}
	time = atoi(argv[2]);
	strcpy(msg, argv[1]);
	signal(SIGALRM, my_alarm);
	alarm(time);
	printf("Entering infinite loop\n");
	while(1) { sleep(10); }
	printf("Can't get here\n");
}

void my_alarm()
{
	printf("%s\n", msg);
	exit(0);
}
