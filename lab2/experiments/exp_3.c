#include <stdio.h>

int main(){
	int ret;
	ret = fork();
	if(ret == 0){
		//child
		printf("The child process ID is %d\n", getpid());
		printf("The child's parent process ID is %d\n", getppid());
	}
	else{
		//parent
		printf("The parent process ID is %d\n", getpid());
		printf("The parent's parent process ID is %d\n", getppid());
	}
	sleep(2);
}
