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
#include <math.h>

#define FILENAME "inputFile.txt"


/* Function prototypes */
int getTotal();
int createSharedArray(int);
void fetchSharedArray(int);
void INThandler(int);
void startTheForking(int);
int createControlArray(int, int, int);
int editControlArray(int, int);
void refactor();
int makeSemaphore();
void doStuff();
void forkOffSomeMore(double, int);
void refactorArray(int, int);

/* Global variables */
int globalTotal;
int* currentProcesses;


int main(int argc, const char* argv[]){
	/* Option variables */
	int options;
	int adjust = 100;
	int sleepTime = 1;

	/* OPTIONS */
	while ((options = getopt(argc, argv, "hra:")) != -1) {

		switch (options) {
		case 'h':
			printf("HOW TO RUN\n");
			printf("Run command './master' after compiling.\n");
			printf("OPTIONS:\n");
			printf("-h Describe how to run, options, then terminate.\n");
			printf("Open the README for instructions on creating an input\n");
			printf("file using the provided program, 'numbers'. If you're\n");
			printf("providing your own file it must be named 'inputFile.txt'\n");
			printf("-r Remove wait time surrounding critical section access.\n");
			printf("-a # Adjust the time (in seconds) allowed for computation. Default is 100.\n");
			exit(0);
		case 'r':
			sleepTime = 0;
			break;
		case 'a':
			adjust = atoi(optarg);
			break;		
		default:
			exit(0);
		}
	}
	int interval = (adjust * 1000);
	
	/* timeout timer timing times timely */
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) doStuff) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = interval / 1000;
	it_val.it_value.tv_usec = (interval * 1000) % 1000000;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}

	/* Clear adder_log */
	refactor();
	
	/* Make semaphore */
	int sem = makeSemaphore();

	/* -------------------------
	     Set signal handler 
	---------------------------*/
	signal(SIGINT, INThandler);

	/* Get number of integers from file */
	int total = getTotal();
	globalTotal = total;

	/* Get log group number */
	double numbersEach = ceil(log(total));
	double groups = ceil(total / numbersEach);
	
	/* Create shared memory array and feed integers from file */
	int sharedMemoryId = createSharedArray(total);

	/* Create control shared memory array and assign values */
	int mode = 0;
	int controlMemoryId = createControlArray(total, mode, sleepTime);

	/* Priliminary forking */
	startTheForking(total);

	/* Create shared memory array and feed integers from file */
	sharedMemoryId = createSharedArray(total);

	/* Set control shared memory array to assigned values */
	mode = 1;
	controlMemoryId = createControlArray(total, mode, sleepTime);

	/* Log forking */
	forkOffSomeMore(groups, numbersEach);

	/* Zero out irrelevant numbers */
	refactorArray(numbersEach, total);

	/* Set control shared memory array to assigned values */
	mode = 0;
	controlMemoryId = createControlArray(total, mode, sleepTime);

	/* Post log forking */
	startTheForking(total);

	/* Remove shared memory*/
	shmdt(currentProcesses);
	shmctl(sharedMemoryId, IPC_RMID, NULL);
	shmctl(controlMemoryId, IPC_RMID, NULL);
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}

return 0;
}

/* Function to determine number of integers in file */
int getTotal() {

	int total = 0;
	/* Open file */
	FILE* file;
	char* mode = "r";
	file = fopen(FILENAME, mode);
	/* Count number of integers in file */
	char tempVariable[100][1];
	while (fscanf(file, "%s", tempVariable[0]) != EOF)	total++;
	/* Close file */
	fclose(file);

	return total;
}

int createSharedArray(int total) {
	/*---------------------------------------
	Set up shared memory array of size 'total'
	----------------------------------------*/
	int* array1;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(0101, total * sizeof(int), 0666 | IPC_CREAT);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array1 = (int*)shmat(shmidAr1, NULL, 0);
	/* Open our file */
	FILE* file;
	char* mode = "r";
	file = fopen("inputFile.txt", mode);
	/* Put integers from file into local array */
	int i = 0;
	while (fscanf(file, "%d", &array1[i]) != EOF) i++;
	/* Close our file*/
	fclose(file);
	/* Detach our array from shared memory */
	shmdt(array1);
	/*-------------------------------------*/
	
	return shmidAr1;
}

int createControlArray(int total, int mode, int sleepTime) {
	int gap = 1;
	int numberToFork = (total / 2);
	int remaining = numberToFork;
	int fields = 7;

	/*---------------------------------------
	Set up shared memory array of size 'total'
	----------------------------------------*/
	int* array2;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr2 = shmget(0202, fields * sizeof(int), 0666 | IPC_CREAT);
	/* Check for errors */
	if (shmidAr2 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array2 = (int*)shmat(shmidAr2, NULL, 0);
	
	/* Set control array variables */
	array2[0] = gap;
	array2[1] = numberToFork;
	array2[2] = total;
	array2[3] = remaining;
	array2[4] = 1; //this will keep track of 'master' process and 'master children'
	array2[5] = mode; //mode: 0 is for sum, 1 is for log
	array2[6] = sleepTime;

	/* Detach our array from shared memory */
	shmdt(array2);
	/*-------------------------------------*/

	return shmidAr2;
}

int editControlArray(int total, int mode) {
	int gap = 1;
	int numberToFork = (total / 2);
	int remaining = numberToFork;
	int fields = 6;

	/*---------------------------------------
	Set up shared memory array of size 'total'
	----------------------------------------*/
	int* array2;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr2 = shmget(0202, fields * sizeof(int), 0666);
	/* Check for errors */
	if (shmidAr2 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array2 = (int*)shmat(shmidAr2, NULL, 0);

	/* Set control array variables */
	array2[0] = gap;
	array2[1] = numberToFork;
	array2[2] = total;
	array2[3] = remaining;
	array2[4] = 1; //this will keep track of 'master' process and 'master children'
	array2[5] = mode; //mode: 0 is for sum, 1 is for log

	/* Detach our array from shared memory */
	shmdt(array2);
	/*-------------------------------------*/

	return shmidAr2;
}

void fetchSharedArray(int total) {
	/* ------------------------------------------
		Pull integers out of shared memory 
	-------------------------------------------*/
	int* array;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr = shmget(0101, total * sizeof(int), 066);
	/* Check for errors */
	if (shmidAr == -1) {
		perror("error getting shmidAr\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array = (int*)shmat(shmidAr, NULL, 0);
	/* Print off integers in shared memory */
	int i;
	for (i = 0; i < total; i++) {
		printf("%d\n", array[i]);
	}
	/* Detach array from shared memory */
	shmdt(array);
}

void  INThandler(int sig)
{
	int fields = 4;
	int sharedMemoryId = shmget(0101, globalTotal * sizeof(int), 0666);
	int controlMemoryId = shmget(0202, fields * sizeof(int), 0666);
	int sem = semget(8576, 1, 0666);
	int processingPROCESSING = shmget(7890, 1024, 0666);
	int shmidAr = shmget(9991, globalTotal * sizeof(int), 0666);

	signal(sig, SIG_IGN);
	shmctl(shmidAr, IPC_RMID, NULL);
	shmctl(sharedMemoryId, IPC_RMID, NULL);
	shmctl(controlMemoryId, IPC_RMID, NULL);
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}
	printf("\nCtrl + C detected. Master process shutting down. Beep. Boop.\n");
	exit(0);

}

void startTheForking(int total) {
	int numberOfProcesses = 0;
	int childrenWorking = 0;
	int letsWait = (total / 2);
	/* Test forking apporpriate amount of children */
	int pid;
	int childIndex = -2;
	
	/*--------------------------------------------------
						 THE LOOP
	--------------------------------------------------*/
	while (numberOfProcesses < letsWait) {
		
		if (childrenWorking < 19) {
			childIndex += 2;
			numberOfProcesses++;
			childrenWorking++;
			pid = fork();
			if (pid < 0) {
				perror("There was a forking error.\n");
				exit(1);
			}
			else if (pid == 0) {
				char* command = "./bin_adder";
				char passChildIndex[32];
				char size[32];
				snprintf(passChildIndex, sizeof(passChildIndex), "%d", childIndex);
				snprintf(size, sizeof(size), "%d", 2);
				int value = execlp(command, passChildIndex, size, (char*)NULL);
				if (value < 0) {
					perror("Error with exec().\n");
					exit(1);
				}
			}

		}
		else {
				wait();
				childrenWorking--;
		}
		
	}
	/*--------------------------------------------------*/

	int k;
	for (k = 0; k < letsWait; k++) {
		wait();
	}
}

void refactor() {
	/* Clear adder_log */
	FILE* file;
	file = fopen("adder_log", "w");
	fclose(file);
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


void doStuff() {
	int fields = 4;
	int sharedMemoryId = shmget(0101, globalTotal * sizeof(int), 0666);
	int controlMemoryId = shmget(0202, fields * sizeof(int), 0666);
	int sem = semget(8576, 1, 0666);
	int processingPROCESSING = shmget(7890, 1024, 0666);
	int shmidAr = shmget(9991, globalTotal * sizeof(int), 0666);

	

	shmctl(shmidAr, IPC_RMID, NULL);
	shmctl(sharedMemoryId, IPC_RMID, NULL);
	shmctl(controlMemoryId, IPC_RMID, NULL);
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}
	printf("\nTimeout detected. Master process shutting down. Boop. Beep.\n");
	exit(0);
}

void forkOffSomeMore(double groups, int number) {
	int numberOfProcesses = 0;
	int childrenWorking = 0;
	
	/* Test forking apporpriate amount of children */
	int pid;
	int childIndex = -number;

	/*--------------------------------------------------
						 THE LOOP
	--------------------------------------------------*/
	while (numberOfProcesses < groups) {

		if (childrenWorking < 19) {
			childIndex += number;
			numberOfProcesses++;
			childrenWorking++;
			pid = fork();
			if (pid < 0) {
				perror("There was a forking error.\n");
				exit(1);
			}
			else if (pid == 0) {
				char* command = "./bin_adder";
				char passChildIndex[32];
				char size[32];
				snprintf(passChildIndex, sizeof(passChildIndex), "%d", childIndex);
				snprintf(size, sizeof(size), "%d", number);
				int value = execlp(command, passChildIndex, size, (char*)NULL);
				if (value < 0) {
					perror("Error with exec().\n");
					exit(1);
				}
			}

		}
		else {
			wait();
			childrenWorking--;
		}

	}
	/*--------------------------------------------------*/

	int k;
	for (k = 0; k < groups; k++) {
		wait();
	}
}

int createSharedArrayLog(int total) {
	/*---------------------------------------
	Set up shared memory array of size 'total'
	----------------------------------------*/
	int* array1;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(2020, total * sizeof(int), 0666 | IPC_CREAT);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array1 = (int*)shmat(shmidAr1, NULL, 0);
	/* Open our file */
	FILE* file;
	char* mode = "r";
	file = fopen("inputFile.txt", mode);
	/* Put integers from file into local array */
	int i = 0;
	while (fscanf(file, "%d", &array1[i]) != EOF) i++;
	/* Close our file*/
	fclose(file);
	/* Detach our array from shared memory */
	shmdt(array1);
	/*-------------------------------------*/

	return shmidAr1;
}

void refactorArray(int numbers, int total) {
	/* ------------------------------------------
		Pull integers out of shared memory
	-------------------------------------------*/
	int* array;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr = shmget(0101, total * sizeof(int), 066);
	/* Check for errors */
	if (shmidAr == -1) {
		perror("error getting shmidAr\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array = (int*)shmat(shmidAr, NULL, 0);
	/* Print off integers in shared memory */
	int i;
	for (i = 1; i < total; i++) {
		if (i % numbers == 0);
		else
			array[i] = 0;
	}
	/* Detach array from shared memory */
	shmdt(array);
}