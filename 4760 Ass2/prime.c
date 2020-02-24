#include "oss.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

void terminator(int *now, int start) {
	
}

int main(int argc, char* argv[]){
	/*get starting time in nanoseconds*/
	int shmidCn = shmget(789, 1024, 0666);
	if (shmidCn == -1) {
		printf("error getting shmid\n");
		exit(1);
	}
	int* startingNano = (int*)shmat(shmidCn, NULL, 0);
	int save = *startingNano;
	printf("Starting time in nanoseconds is %d\n", *startingNano);
//printf("I am prime.c\n");
//printf("%d\n", argv[0]);
printf("I am child %d.\n", atoi(argv[0]));
//printf("%d\n", argc);

int shmid = shmget(123, 1024, 0666);
if (shmid == -1){
	printf("error getting shmid\n");
	exit(1);
}


int *pull = (int*) shmat(shmid, NULL, 0);

printf("My shared memory integer is %d\n", *pull);

printf("I will check %d for primality.\n", atoi(argv[1]));

printf("Child %d is done now.\n\n", atoi(argv[0]));


int shmidNow = shmget(789, 1024, 0666);
if (shmidNow == -1) {
	printf("error getting shmid\n");
	exit(1);
}
int* now = (int*)shmat(shmidNow, NULL, 0);
printf("calculation is %d - %d which is %d\n", *now, save, (*now - save));



int* array;

int shmidAr = shmget(0101, 1024, 066);
if (shmidAr == -1) {
	printf("error getting shmid\n");
	exit(1);
}
array = (int*)shmat(shmidAr, NULL, 0);
array[atoi(argv[0])] = 1;

shmdt(array);
shmdt(pull);
shmdt(startingNano);

return 0;

}
