#include <signal.h>
#include <stdio.h>

int ret;

main()
{
	ret = fork();
	if(ret == 0)
	{
		signal(SIGINT,SIG_IGN);
		sleep(2);
		printf("Child %d with parent %d\n", getpid(), getppid());
	}
	else
	{
		sleep(1);
		kill(ret,SIGINT);
		printf("Parent %d with child %d\n", getpid(), ret);
		wait(0);
	}
}
