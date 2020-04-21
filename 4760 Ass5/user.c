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
void makeRequest(int);
void releaseRequest(int);
int checkRequest();
unsigned long int fetchSharedNanoSeconds();
void editSharedNanoSeconds(unsigned long int);
int myProcessNumber;
int myRequest;
int myRequest1 = 0;
int myRequest2 = 0;
int myRequest3 = 0;
void semBegin();
void semEnd();
int reqNum;

int main(int argc, char* argv[]) {
	/*following should terminate self if parent terminates*/
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	//itsa me, commentio
	myProcessNumber = atoi(argv[0]);
	srand(getpid());
	int bound = (rand() % (500 - 1 + 1)) + 1;
	reqNum = 3;// (rand() % (3 - 1 + 1)) + 1;
	/* get current time */
	unsigned long int currentTime = fetchSharedNanoSeconds();
	//printf("we dont even get here\n");
	unsigned long int one = currentTime + (bound * 1000000);
	unsigned long int two = currentTime + (bound * 1000000 * 2);
	unsigned long int three = currentTime + (bound * 1000000 * 3);
	int track = 1;
	
	while (1) {
		//printf("%d", myProcessNumber);

		semBegin();
		editSharedNanoSeconds(1000000);
		semEnd();

		if (track == 1) {
			unsigned long int timTheTimeManTaylor = fetchSharedNanoSeconds();
			if (timTheTimeManTaylor >= one) {

				myRequest = (rand() % (19 - 0 + 1)) + 0;
				myRequest1 = myRequest;
				//myRequest = 0;
				makeRequest(1);
				if (reqNum == 1)
					break;
				else
					track++;
			}
		}
		if (track == 2) {
			unsigned long int timTheTimeManTaylor = fetchSharedNanoSeconds();
			if (timTheTimeManTaylor >= two) {

				myRequest = (rand() % (19 - 0 + 1)) + 0;
				myRequest2 = myRequest;
				//myRequest = 0;
				makeRequest(2);
				if (reqNum == 2)
					break;
				else
					track++;
			}
		}
		if (track == 3) {
			unsigned long int timTheTimeManTaylor = fetchSharedNanoSeconds();
			if (timTheTimeManTaylor >= three) {

				myRequest = (rand() % (19 - 0 + 1)) + 0;
				myRequest3 = myRequest;
				//myRequest = 0;
				makeRequest(3);
				if (reqNum == 3)
					break;
				else
					track++;
			}
		}
	}
	
	/* Choose resource to request */
	//myRequest = (rand() % (19 - 0 + 1)) + 0;
	//myRequest = 0;
	//makeRequest(1);
	/* Loop until request granted */
	int x = 0;
	/* Process "sleeps" until request is granted */
	while (1) {
		/* If return value from checkRequest is 1, request was granted */
		int value = checkRequest();
		if (value == 1)
			break;
		x++;
		//if (x > 10000)
			//break;
		/*
		if (x > 100) {
			sleep(1);
			releaseRequest(reqNum);
			editCurrentNOP(-1);
			printf("P%d pseudo deadlock detected\n", myProcessNumber);
			sleep(1);
			exit(1);
		}
		*/
	}
	sleep(1);
	releaseRequest(reqNum);
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

void makeRequest(int requestNumber) {
	int thisRequest;
	if (requestNumber == 1) {
		thisRequest = myRequest1;
	}
	if (requestNumber == 2) {
		thisRequest = myRequest2;
	}
	if (requestNumber == 3) {
		thisRequest = myRequest3;
	}
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

		if (resourceDescriptorArray[thisRequest].requested[i] == -1) {
			resourceDescriptorArray[thisRequest].requested[i] = myProcessNumber;
			//printf("Request made by P%d for R%d\n", myProcessNumber, myRequest);
			break;
		}

	}
	shmdt(resourceDescriptorArray);
}

void releaseRequest(int reqNum) {
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

	if (reqNum == 1) {
		resourceDescriptorArray[myRequest1].allocated--;
		resourceDescriptorArray[myRequest1].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest1);
	}
	if (reqNum == 2) {
		resourceDescriptorArray[myRequest1].allocated--;
		resourceDescriptorArray[myRequest1].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest1);
		resourceDescriptorArray[myRequest2].allocated--;
		resourceDescriptorArray[myRequest2].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest2);
	}
	if (reqNum == 3) {
		resourceDescriptorArray[myRequest1].allocated--;
		resourceDescriptorArray[myRequest1].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest1);
		resourceDescriptorArray[myRequest2].allocated--;
		resourceDescriptorArray[myRequest2].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest2);
		resourceDescriptorArray[myRequest3].allocated--;
		resourceDescriptorArray[myRequest3].available++;
		printf("P%d released R%d\n", myProcessNumber, myRequest3);
	}

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

		if (resourceDescriptorArray[myRequest1].requested[i] == myProcessNumber) {
			/* Request has NOT been granted */
			value = 0;			
		}
		if (resourceDescriptorArray[myRequest2].requested[i] == myProcessNumber) {
			/* Request has NOT been granted */
			value = 0;
		}
		if (resourceDescriptorArray[myRequest3].requested[i] == myProcessNumber) {
			/* Request has NOT been granted */
			value = 0;
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

void editSharedNanoSeconds(unsigned long int increment) {
	/*---------------------------------------
	Edit nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error88\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds = (unsigned long int*)(paddrNS);
	*nanoSeconds += increment;

	// detach from shared memory
	shmdt(nanoSeconds);
}

void semBegin() {
	/* Fetch semaphore */
	int sem = semget(8576, 1, 0666);
	if (sem == -1) {
		perror("main: semget");
		exit(1);
	}
	/* structure for semaphore operations.   */
	struct sembuf sem_op;

	/* wait on the semaphore, unless it's value is non-negative. */
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	semop(sem, &sem_op, 1);
}

void semEnd() {
	/* Fetch semaphore */
	int sem = semget(8576, 1, 0666);
	if (sem == -1) {
		perror("main: semget");
		exit(1);
	}
	/* structure for semaphore operations.   */
	struct sembuf sem_op;
	/* finally, signal the semaphore - increase its value by one. */
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	semop(sem, &sem_op, 1);
}