#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_message(){
	int index;
	static char message[][sizeof("The penguins are coming!\n")]={
		"Hello Iowa!\n",
		"Goodbye Iowa!\n",
		"The penguins are coming!\n",
		"Caffeine is my friend!\n"
		};
	srand(time((time_t *)0));
	index=rand()/(RAND_MAX/4);
	printf("%s",message[index]);
}
