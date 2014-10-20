#include <stdio.h>

int main(){
	int i=0, j=0, ret;
	ret = fork();
	if (ret==0){
		for(i=0; i<500000; i++)
			printf("Child: %d\n", i);
		printf("Child ends\n");
	}
	else{
		wait(0);
		printf("Parent resumes.\n");
		for(j=0; j<500000; j++)
			printf("Parent: %d\n", j);
	}
}
