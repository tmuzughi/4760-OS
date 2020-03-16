#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <math.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <sys/sem.h>



/* Function prototypes */
int binSum();
int binLog(int*, int, int, int);
int* fetchSharedArray(int);
int* fetchControlArray();
void startTheForking(int, int, int, int);
void update_file(int, int, int, int, int);
void oneRingToRuleThemAll();

/* Global variables */
int* timeoutCheck;

int main(int argc, const char* argv[]){
	/*following should terminate self if parent terminates*/
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	
	/* Assign child its index */
	int child = atoi(argv[0]);
	int number = atoi(argv[1]);

	/* Fetch control array and set local control variables */
	int* controlArray = fetchControlArray();
	int gap = controlArray[0];
	int numberToFork = controlArray[1];
	int total = controlArray[2];
	int mode = controlArray[5];
	int sleepTime = controlArray[6];

	/* Fetch shared memory array */
	int* array = fetchSharedArray(total);

	/* CHECK MODE */
	if (mode == 0) {
		/* Put sum into correct position in shared memory array */
		array[child] = binSum(array[child], array[child + gap]);
	}
	else
	{
		/* Put sum into correct position in shared memory array */
		array[child] = binLog(array, child, number, total);
		
	}

	/* Fetch semaphore */
	int sem = semget(8576, 1, 0666);
	if (sem == -1) {
		perror("main: semget");
		exit(1);
	}
	/* Update adder_log */
	update_file(sem, child, array[child], number, sleepTime);

	/* If doing log summation, exit */
	if (mode != 0) {
		exit(0);
	}

	/* If 'master child' wait for siblings to die, otherwise decrement 'remaining' */
	if (child == 0) {
		while (controlArray[3] > 1);
		/* Set new values for control array */
		controlArray[0] = (gap * 2);
			gap = controlArray[0];
		if (numberToFork > 1)
			controlArray[1] = (numberToFork / 2);
		else
			controlArray[1] = 0;
		numberToFork = controlArray[1];
		controlArray[2] = (total / 2);
			total = controlArray[2];
		controlArray[3] = numberToFork;
		int minus = controlArray[4];
		/* Fork off new children */
		controlArray[4] = (minus + 1);
		startTheForking(gap, numberToFork, total, minus);
		
	}
	else 
		controlArray[3] -= 1;

return 0;
}

int binSum(int first, int second) {
	return (first + second);
}

int binLog(int* array, int child, int number, int total) {
	/* Quick edition to acount for remainders */
	if ((total - child) < number) {
		number = (total - child);
	}
	/* Get the sum */
	int sum = 0;
	int i;
	for (i = 0; i < number; i++) {		
		sum += array[child + i];
	}
	return sum;
}

int* fetchSharedArray(int total) {
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

	return array;
}

int* fetchControlArray() {
	/* ------------------------------------------
		Pull integers out of shared memory
	-------------------------------------------*/
	int* array;
	int fields = 7;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(0202, fields * sizeof(int), 066);
	/* Check for errors */
	if (shmidAr1 == -1) {
		perror("error getting shmidAr1. bin\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array = (int*)shmat(shmidAr1, NULL, 0);

	return array;
}

void startTheForking(int gap, int numberToFork, int total, int minus) {
	/* Test forking appropriate amount of children */
	int currentProcesses = 0;
	int childrenWorking = 0;
	int pid;
	int childIndex = -(gap * 2);
	int i;
	while (currentProcesses < numberToFork) {	
		/* Timeout check */
		if (timeoutCheck != 0) {
			exit(1);
		}
		if (childrenWorking < (19 - minus)) {
			currentProcesses++;
				childrenWorking++;
			childIndex += (gap * 2);
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
		else
		{
			wait();
			childrenWorking--;
		}
	}
	for (i = 0; i < numberToFork; i++) {
		wait();
	}
}

/* this function updates the contents of the file with the given path name. */
void update_file(int sem, int index, int number, int size, int sleepTime)
{
	/* structure for semaphore operations.   */
	struct sembuf sem_op;
		
	FILE* file;

	/* wait on the semaphore, unless it's value is non-negative. */
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;  
	sem_op.sem_flg = 0;
	semop(sem, &sem_op, 1);

	/* wait for 1 second */
	sleep(sleepTime);

	time_t curtime;
	time(&curtime);
	fprintf(stderr, "Entering critical section at ");
	fprintf(stderr, "%s\n", ctime(&curtime));

	/* "locked" the semaphore to assure exclusive access to file.  */
	file = fopen("adder_log", "a");

	if (file) {
		fprintf(file, "PID    Index     Size\n");
		fprintf(file, "%d     %d        %d\n", getpid(), index, size);
		fprintf(file, "Calculated %d as sum\n", number);
		fclose(file);
	}

	/* wait for 1 second */
	sleep(sleepTime);

	time(&curtime);
	fprintf(stderr, "Exiting critical section at ");
	fprintf(stderr, "%s\n", ctime(&curtime));

	/* finally, signal the semaphore - increase its value by one. */
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;  
	sem_op.sem_flg = 0;
	semop(sem, &sem_op, 1);
}

void oneRingToRuleThemAll() {
	/*---------------------------------------
	Test integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
	int shmid = shmget(7890, 1024, 0666);

	// error check for shmget
	if (shmid < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddr = (char*)shmat(shmid, NULL, 0);
	timeoutCheck = (int*)(paddr);

	/*---------------------------------------*/
}