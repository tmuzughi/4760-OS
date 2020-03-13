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


/* Function prototypes */
int binSum();
void binLog();
int* fetchSharedArray(int);
int* fetchControlArray();
void startTheForking(int, int, int);

int main(int argc, const char* argv[]){
	/* Assign child its index */
	int child = atoi(argv[0]);

	/* Fetch control array and set control variables */
	int* controlArray = fetchControlArray();
	int gap = controlArray[0];
	int numberToFork = controlArray[1];
	int total = controlArray[2];

	/* Test control array */
	if (child == 0)
		printf("%d %d %d %d\n", controlArray[0], controlArray[1], controlArray[2], controlArray[3]);

	/* Fetch shared memory array */
	int* array = fetchSharedArray(total);

	/* Put sum into correct position in shared memory array */
	array[child] = binSum(array[child], array[child + gap]);

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
		/* Fork off new children */
		startTheForking(gap, numberToFork, total);
	}
	else 
		controlArray[3] -= 1;

	/* Detach array from shared memory */
	shmdt(array);
	shmdt(controlArray);
return 0;
}

int binSum(int first, int second) {
	return (first + second);
}

void binLog() {

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
	int fields = 4;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(0202, fields * sizeof(int), 066);
	/* Check for errors */
	if (shmidAr1 == -1) {
		perror("error getting shmidAr1\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array = (int*)shmat(shmidAr1, NULL, 0);

	return array;
}

void startTheForking(int gap, int numberToFork, int total) {
	/* Test forking appropriate amount of children */
	int pid;
	int childIndex = -(gap * 2);
	int i;
	for (i = 0; i < numberToFork; i++) {
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
	for (i = 0; i < numberToFork; i++) {
		wait();
	}
	printf("All my friends are dead too!\n");
}