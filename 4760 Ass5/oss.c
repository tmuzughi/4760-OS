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
#include<sys/wait.h>
#include "oss.h"

#define maxBhatia 18
	
/*----------------------------------------
			FUNCTION PROTOTYPES
----------------------------------------*/
int createSharedSeconds();
void editSharedSeconds(int);
int fetchSharedSeconds();
int createSharedNanoSeconds();
void editSharedNanoSeconds(int);
unsigned long int fetchSharedNanoSeconds();
void forkOneProcess();
int createResourceDescriptors();
void func(int);
int createCurrentNOP();
void editCurrentNOP(int);
int fetchCurrentNOP();
void INThandler(int);
void handleRequests();
int makeSemaphore();
void semBegin();
void semEnd();

/*----------------------------------------
				GLOBALS
----------------------------------------*/
int currentNOP = 0; //Number of Processes that exist now
int maxCurrentNOP = maxBhatia; //Maximum Simultaneous Number Of Processes
int totalNOP = 0; //How many processes exist cumulative
int maxTotalNOP = 30; //Total processes we will create

FILE* file;

int main(int argc, char* argv[]){
	
	file = fopen("output_log", "w");
	fclose(file);
	signal(SIGCHLD, func);
	signal(SIGINT, INThandler);
	/* Make semaphore */
	int sem = makeSemaphore();

	/* Create Resource Descriptors */
	int resourceDescriptorArrayID = createResourceDescriptors();

	/* Create shared nano seconds */
	int sharedNanoID = createSharedNanoSeconds();

	/* Create current NOP ID */
	int currentNOPID = createCurrentNOP();

	unsigned long int timeToCreate = 0;
	
	int times = 1;
	srand(getpid());
	while (totalNOP < maxTotalNOP) {
		
		/* Increment the nanoseconds */
		semBegin();
		editSharedNanoSeconds(1000000);
		semEnd();
		/* Check Requests */
		handleRequests();
		/* Get shared nanoseconds */
		unsigned long int currentNanos = fetchSharedNanoSeconds();

		currentNOP = fetchCurrentNOP();
		if (currentNOP < 18) {
			
			/* If nanos are greater than threshold, activate forking! */
			if (currentNanos >= (timeToCreate)) {
				timeToCreate = fetchSharedNanoSeconds();
				timeToCreate += (rand() % (500000000 - 1000000 + 1)) + 1000000;
				forkOneProcess();
				editCurrentNOP(1);
				totalNOP++;
				times++;
			}
		}
		else
		{
			int b = 0;
			//int numby = (rand() % (18 - 2 + 1)) + 2;
			//for (b = 0; b < 18; b++) {
				//handleRequests;
				//wait(NULL);
			//}
			while (wait(NULL) > 0) {
				handleRequests();
				}
			//while (currentNOP >= 18) {
				//b++;
				//handleRequests();
				//if (b > 1000)
					//break;					
			//}
		}		
	}
	//handleRequests();
	/* "Wait" for all remaining user processes to terminate */
	int k;
	for (k = 0; k < maxTotalNOP; k++) {
		handleRequests();
		wait(NULL);
	}
		
	/* Remove shared memory */
	shmctl(sharedNanoID, IPC_RMID, NULL);
	shmctl(resourceDescriptorArrayID, IPC_RMID, NULL);
	shmctl(currentNOPID, IPC_RMID, NULL);
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}

return 0;
}

int createSharedSeconds() {
	/*---------------------------------------
		Make seconds integer for clock
			in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 0666 | IPC_CREAT);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds = (unsigned long int*)(paddrS);
	*seconds = 0;

	// detach from shared memory
	shmdt(seconds);

	return shmidS;
}

void editSharedSeconds(int increment) {
	/*---------------------------------------
		Edit seconds integer for clock
			in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds = (unsigned long int*)(paddrS);
	*seconds += increment;

	// detach from shared memory
	shmdt(seconds);

}

int fetchSharedSeconds() {
	/*---------------------------------------
		Fetch seconds integer for clock
			in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds;
	seconds = (unsigned long int*)(paddrS);
	//printf("The number of seconds is %d\n", *seconds);
	int secondsPass = *seconds;
	// detach from shared memory
	shmdt(seconds);

	return secondsPass;
}

int createSharedNanoSeconds() {
	/*---------------------------------------
	Make nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 0666 | IPC_CREAT);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds = (unsigned long int*)(paddrNS);
	*nanoSeconds = 0;

	// detach from shared memory
	shmdt(nanoSeconds);

	return shmidNS;
}

void editSharedNanoSeconds(increment) {
	/*---------------------------------------
	Edit nano seconds integer for clock
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
	unsigned long int* nanoSeconds = (unsigned long int*)(paddrNS);
	*nanoSeconds += increment;

	// detach from shared memory
	shmdt(nanoSeconds);
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

void forkOneProcess() {
	//printf("Current # of processes are %d\n", currentNOP);
	int pid = fork();
	if (pid != 0) {

	}
	if (pid < 0) {
		perror("There was a forking error.\n");
		printf("Current processes %d\n", currentNOP);
		printf("Total processes %d\n", totalNOP);
		int shmid = shmget(77789, 1024, 066);
		int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 066);
		int shmidNS = shmget(789, sizeof(unsigned long int), 066);
		//int shmidS = shmget(678, sizeof(unsigned long int), 066);

		//signal(sig, SIG_IGN);
		shmctl(shmid, IPC_RMID, NULL);
		shmctl(shmidAr1, IPC_RMID, NULL);
		shmctl(shmidNS, IPC_RMID, NULL);
		//shmctl(shmidS, IPC_RMID, NULL);
		exit(1);
	}
	else if (pid == 0) {
		//printf("goodbye, cruel world\n");
		
		char* command = "./user";
		char isRealPass[32];
		snprintf(isRealPass, sizeof(isRealPass), "%d", totalNOP);

		int value = execlp(command, isRealPass, (char*)NULL);
		if (value < 0) {
			perror("Error with exec().\n");
			exit(1);
		}
	}
}

int createResourceDescriptors() {
	resourceDescriptor* resourceDescriptorArray;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 0666 | IPC_CREAT);
	
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on shmidAr1\n");
		exit(1);
	}
	/* Attach shared memory */
	resourceDescriptorArray = (resourceDescriptor*)shmat(shmidAr1, NULL, 0);

	/* Set R 0-4 to # of instances between 2-10 */
	srand(getpid());
	int i;
	for (i = 0; i < 5; i++) {
		resourceDescriptorArray[i].total = 999;
	}
	/* Determine % of resources which will be sharable */
	int percent = (rand() % (5 - 3 + 1)) + 3;

	/* Adjust R 3, 4 accordingly */
	if (percent == 4)
		resourceDescriptorArray[4].total = (rand() % (10 - 1 + 1)) + 1;
	if (percent == 3) {
		resourceDescriptorArray[4].total = (rand() % (10 - 1 + 1)) + 1;
		resourceDescriptorArray[3].total = (rand() % (10 - 1 + 1)) + 1;
	}		
	/* Instantiate remaining RDs with 1 (non-sharable) */
	for (i = 5; i < 20; i++) {
		resourceDescriptorArray[i].total = (rand() % (10 - 1 + 1)) + 1;
	}
	file = fopen("output_log", "a");	
	/* PRINT FOR TESTING */
	fprintf(file, "Percent is %d%\n", ((percent * 100) / 20));
	for (i = 0; i < 20; i++) {
		fprintf(file, "R%d has %d instance(s)\n", i, resourceDescriptorArray[i].total);
	}
	/* Initialize requested array for each descriptor */
	int a;
	int e;
	for (a = 0; a < 20; a++) {
		for (e = 0; e < 20; e++) {
			resourceDescriptorArray[a].requested[e] = -1;
			resourceDescriptorArray[a].available = resourceDescriptorArray[a].total;
		}
	}
	/* Detach from shared memory */
	shmdt(resourceDescriptorArray);

	return shmidAr1;
}

void handleRequests() {
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
	//printf("%d\n", resourceDescriptorArray[0].requested[0]);
	/* Check for requests */
	int a;
	int e;
	//printf("loop\n");
	for (a = 0; a < 20; a++) {
		for (e = 0; e < 20; e++) {
			if (resourceDescriptorArray[a].requested[e] > -1) {
				/* Request found */
				printf("P%d has requested R%d\n", resourceDescriptorArray[a].requested[e], a);
				/* Allocate, if resource is avaialable */
				
				if (resourceDescriptorArray[a].available > 0) {
					resourceDescriptorArray[a].allocated++;
					resourceDescriptorArray[a].available--;
					/* Remove request */
					resourceDescriptorArray[a].requested[e] = -1;
				}
				else {
					printf("Request denied\n");
				}
				
				
			}		
		}
	}
	shmdt(resourceDescriptorArray);

}

void func(int signum)
{
	//currentNOP--;
}

int createCurrentNOP() {
/*---------------------------------------
	NOP integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
	int shmid = shmget(77789, 1024, 0666 | IPC_CREAT);

	// error check for shmget
	if (shmid < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddr = (char*)shmat(shmid, NULL, 0);
	int* pint = (int*)(paddr);
	*pint = 0;
	// detach from shared memory
	shmdt(paddr);
	shmdt(pint);

	return shmid;
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

int fetchCurrentNOP() {
	/*---------------------------------------
	NOP integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
	int shmid = shmget(77789, 1024, 066);
	int number;
	// error check for shmget
	if (shmid < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddr = (char*)shmat(shmid, NULL, 0);
	int* pint = (int*)(paddr);
	number = *pint;
	// detach from shared memory
	shmdt(paddr);
	shmdt(pint);

	return number;
}

void  INThandler(int sig)
{
	int shmid = shmget(77789, 1024, 066);
	int shmidAr1 = shmget(90210, 20 * sizeof(resourceDescriptor), 066);
	int shmidNS = shmget(789, sizeof(unsigned long int), 066);
	int sem = semget(8576, 1, 0666);
	//int shmidS = shmget(678, sizeof(unsigned long int), 066);

	signal(sig, SIG_IGN);
	shmctl(shmid, IPC_RMID, NULL);
	shmctl(shmidAr1, IPC_RMID, NULL);
	shmctl(shmidNS, IPC_RMID, NULL);
	//shmctl(shmidS, IPC_RMID, NULL);
	
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}
	
	printf("\nCtrl + C detected. Master process shutting down. Beep. Boop.\n");
	exit(0);

}

int makeSemaphore() {
	/* -----------------------------------------------
						SEMAPHORES
	-------------------------------------------------*/
	int sem = semget(8576, 1, IPC_CREAT | 0666);
	if (sem == -1) {
		perror("main: semget");
		exit(1);
	}
	/* use this to store return values of system calls.   */
	int rc;

	/* initialize the first semaphore in our set to '3'.  */
	rc = semctl(sem, 0, SETVAL, 1);
	if (rc == -1) {
		perror("main: semctl");
		exit(1);
	}

	return sem;
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