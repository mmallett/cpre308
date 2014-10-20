#include <stdio.h>
#include <unistd.h>

int main(){
	execl("/bin/ls","ls",0);
	printf("After exec\n");
}
