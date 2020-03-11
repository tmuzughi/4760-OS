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

#define FILENAME "inputFile.txt"

/* Function prototypes */
int getTotal();
void timeToDie(clock_t, clock_t);
int createSharedArray(int);
void fetchSharedArray(int);

int main(int argc, char* argv){
	/* *** TODO: MOVE CLOCK START TO BEGINNING OF COMPUTATIONS ***/
	/*Start 'real time' clock*/
	clock_t start, check;
	start = clock();

	/* Get number of integers from file */
	int total = getTotal();

	/* Create shared memory array and feed integers from file */
	int sharedMemoryId = createSharedArray(total);

	/* Test fetching integers from shared memory */
	fetchSharedArray(total);

	/* Remove shared memory*/
	shmctl(sharedMemoryId, IPC_RMID, NULL);	

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
	double totalTime;
	check = clock();
	totalTime = ((double)(check - start) / CLOCKS_PER_SEC);
	if (totalTime > 100)
	{
		printf("Process master terminated: elapsed time greater than 100 seconds\n");
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