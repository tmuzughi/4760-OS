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
#define INTERVAL 100000

/* Function prototypes */
int getTotal();
void timeToDie(clock_t, clock_t);
int createSharedArray(int);
void fetchSharedArray(int);
void INThandler(int);
void startTheForking(int, clock_t, clock_t);
int createControlArray(int);
void refactor();
int makeSemaphore();
int flagArray();
void doStuff();

/* Global variable */
int globalTotal;
int* currentProcesses;


int main(int argc, const char* argv[]){
	/* timeout timer timing times timely */
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) doStuff) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = INTERVAL / 1000;
	it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	
	/* Clear adder_log */
	refactor();
	
	/* Make semaphore */
	int sem = makeSemaphore();

	/* Create process handler */
	int processingPROCESSING = oneRingToRuleThemAll();

	/* -------------------------
	     Set signal handler 
	---------------------------*/
	signal(SIGINT, INThandler);

	/* Get number of integers from file */
	int total = getTotal();
	globalTotal = total;

	/* Create flag array */
	int flag = flagArray(total);

	/* Get log group number */
	int numbersEach = ceil(log(total));
	int groups = ceil(total / numbersEach);
	//printf("%d %d\n", groups, numbersEach);
	
	/* Create shared memory array and feed integers from file */
	int sharedMemoryId = createSharedArray(total);

	/* Create control shared memory array and assign values */
	int controlMemoryId = createControlArray(total);

	/*Start 'real time' clock*/
	clock_t start, check;
	start = clock();

	
	/* Test forking */
	startTheForking(total, start, check);

	/* Test fetching integers from shared memory */
	fetchSharedArray(total);

	/* Remove shared memory*/
	shmdt(currentProcesses);
	shmctl(processingPROCESSING, IPC_RMID, NULL);
	shmctl(sharedMemoryId, IPC_RMID, NULL);
	shmctl(controlMemoryId, IPC_RMID, NULL);
	shmctl(flag, IPC_RMID, NULL);
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

void timeToDie(clock_t start, clock_t check) {
	/* *** TODO: CALL THIS FUNCTION AT APPROPRIATE SPOT IN LOOP *** */
	/*------------------------------------
				Real Clock
	-------------------------------------*/
	/* I WILL MESS WITH TIME!!! */
	double totalTime;
	check = clock();
	totalTime = ((double)(check - start) / CLOCKS_PER_SEC);
	if (totalTime > 2)
	{
		/* Set marker for child processes to terminate */
		currentProcesses = 1;
		printf("Process master terminated: elapsed time greater than 100 seconds. Boop. Beep.\n");
		exit(1);
	}
	
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

int createControlArray(int total) {
	int gap = 1;
	int numberToFork = (total / 2);
	int remaining = numberToFork;
	int fields = 5;

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
	shmctl(processingPROCESSING, IPC_RMID, NULL);
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

void startTheForking(int total, clock_t start, clock_t check) {
	int numberOfProcesses = 0;
	int childrenWorking = 0;
	int letsWait = (total / 2);
	/* Test forking apporpriate amount of children */
	int pid;
	int childIndex = -2;
	int numberOfStartingProcesses = (total / 2);
	int i;
	
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

	printf("NOP %d\n", numberOfProcesses);
	int k;
	for (k = 0; k < letsWait; k++) {
		wait();
	}
	printf("All my friends are dead.\n");
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

int oneRingToRuleThemAll() {
	/*---------------------------------------
	Test integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
	int shmid = shmget(7890, 1024, 0666 | IPC_CREAT);

	// error check for shmget
	if (shmid < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	currentProcesses = (int*)shmat(shmid, NULL, 0);
	
	currentProcesses = 0;
	
	return shmid;
	/*---------------------------------------*/
}

int flagArray(int total) {
	/*--------------------------------------
		Shared memory flag array
		-------------------------------------- - */
		// shmat to attach to shared memory
		int* array;
	// shmget returns an identifier in shmid
	int shmidAr = shmget(9991, total * sizeof(int), 0666 | IPC_CREAT);
	// error check for shmget
	if (shmidAr < 0) {
		perror("shmget error\n");
		exit(1);
	}
	array = (int*)shmat(shmidAr, NULL, 0);
	int i;
	for (i = 0; i < (total); i++) {
		array[i] = 0;
	}
	// detach from shared memory
	shmdt(array);
	/*-------------------------------------*/
	return shmidAr;
}
void doStuff() {
	int fields = 4;
	int sharedMemoryId = shmget(0101, globalTotal * sizeof(int), 0666);
	int controlMemoryId = shmget(0202, fields * sizeof(int), 0666);
	int sem = semget(8576, 1, 0666);
	int processingPROCESSING = shmget(7890, 1024, 0666);
	int shmidAr = shmget(9991, globalTotal * sizeof(int), 0666);

	
	shmctl(processingPROCESSING, IPC_RMID, NULL);
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