#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>           
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/msg.h> 
#include <math.h>
#include <sys/prctl.h>
#include "oss.h"

	
void editCurrentNOP(int);
void makeRequest();
void releaseRequest();
int checkRequest();
unsigned long int fetchSharedNanoSeconds();
int myProcessNumber;
int myRequest;

int main(int argc, char* argv[]){
	/*following should terminate self if parent terminates*/
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	//itsa me, commentio
	myProcessNumber = atoi(argv[0]);
	srand(getpid());
	int bound = (rand() % (500 - 1 + 1)) + 1;
	int reqNum = (rand() % (3 - 1 + 1)) + 1;
	/* get current time */
	/*
	unsigned long int currentTime = fetchSharedNanoSeconds();
	unsigned long int one = currentTime + (bound * 1000000);
	unsigned long int two = currentTime + (bound * 1000000 * 2);
	unsigned long int three = currentTime + (bound * 1000000 * 3);
	int track = 1;
	while (1) {
		if (track == 1) {
			unsigned long int timTheTimeManTaylor = fetchSharedNanoSeconds();
		}
		if (track == 2) {

		}
		if (track == 3) {

		}
	}
	*/
	/* Choose resource to request */
	myRequest = (rand() % (19 - 0 + 1)) + 0;
	//myRequest = 0;
	makeRequest();
	/* Loop until request granted */
	int x = 0;
	/* Process "sleeps" until request is granted */
	while (1) {
		/* If return value from checkRequest is 1, request was granted */
		int value = checkRequest();
		if (value == 1)
			break;
		//x++;
		//if (x > 10000)
			//break;
	}
	sleep(1);
	releaseRequest();
	sleep(1);
//printf("I am P%d and I want R%d\n", myProcessNumber, myRequest);
editCurrentNOP(-1);

return 0;
}

void editCurrentNOP(int number) {
	/*---------------------------------------
	NOP integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
	int shmid = shmget(77789, 1024, 066);

	// error check for shmget
	if (shmid < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddr = (char*)shmat(shmid, NULL, 0);
	int* pint = (int*)(paddr);
	*pint += number;
	// detach from shared memory
	shmdt(paddr);
	shmdt(pint);

}

void makeRequest() {
	resourceDescriptor* resourceDescriptorArray;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 066);

	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on shmidAr1\n");
		exit(1);
	}
	/* Attach shared memory */
	resourceDescriptorArray = (resourceDescriptor*)shmat(shmidAr1, NULL, 0);

	int i;
	for (i = 0; i < 20; i++) {
		
		if (resourceDescriptorArray[myRequest].requested[i] == -1) {
			resourceDescriptorArray[myRequest].requested[i] = myProcessNumber;
			//printf("Request made by P%d for R%d\n", myProcessNumber, myRequest);
			break;
		}
		
	}
	shmdt(resourceDescriptorArray);
}

void releaseRequest() {
	resourceDescriptor* resourceDescriptorArray;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 066);

	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on shmidAr1\n");
		exit(1);
	}
	/* Attach shared memory */
	resourceDescriptorArray = (resourceDescriptor*)shmat(shmidAr1, NULL, 0);

	resourceDescriptorArray[myRequest].allocated--;
	resourceDescriptorArray[myRequest].available++;
	printf("P%d released R%d\n", myProcessNumber, myRequest);
	shmdt(resourceDescriptorArray);
}

int checkRequest() {
	int value = 1;
	resourceDescriptor* resourceDescriptorArray;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 066);

	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on shmidAr1\n");
		exit(1);
	}
	/* Attach shared memory */
	resourceDescriptorArray = (resourceDescriptor*)shmat(shmidAr1, NULL, 0);

	int i;
	for (i = 0; i < 20; i++) {

		if (resourceDescriptorArray[myRequest].requested[i] == myProcessNumber) {
			/* Request has NOT been granted */
			value = 0;
			break;
		}

	}
	shmdt(resourceDescriptorArray);
	return value;
}

unsigned long int fetchSharedNanoSeconds() {
	/*---------------------------------------
	Fetch nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds;
	nanoSeconds = (unsigned long int*)(paddrNS);
	//printf("The number of nano-seconds is %d\n", *nanoSeconds);
	unsigned long int nanoSecondsPass = *nanoSeconds;

	// detach from shared memory
	shmdt(nanoSeconds);

	return nanoSecondsPass;
}