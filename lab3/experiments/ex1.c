#include <stdio.h>
#include <pthread.h>

//what thread 1 will execute
void* thread1()
{
	sleep(5);
	printf("Hello, I am thread 1\n");
}

//what thread 2 will execute
void* thread2()
{
	sleep(5);
	printf("Hello, I am thread 2\n");
}

int main(int argc, char *argv[])
{
	//declare 2 threads
	pthread_t i1;
	pthread_t i2;

	//create the threads
	pthread_create(&i1, NULL, (void*)&thread1, NULL);
	pthread_create(&i2, NULL, (void*)&thread2, NULL);

	//wait until both threads done
	pthread_join(i1,NULL);
	pthread_join(i2,NULL);

	printf("Hello, I am the main process\n");

	return 0;
}
