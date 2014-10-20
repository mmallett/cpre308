#include <stdio.h>

int main(){
	printf("Process ID is: %d\n", getpid());
	printf("Parent process ID is: %d\n", getppid());
	sleep(120); //sleep for 2 min
	printf("I am awake.\n");
}
