#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>



int main(int argc, char* argv[]){

	/*Start 'real time' clock*/
	clock_t start, check;
	start = clock();

int options;
int nValue = 4;
int sValue = 2;
int bValue = 0;
int iValue = 5;
char outputFile[100];

while((options = getopt(argc, argv, "hnsbio")) != -1){

switch(options){
	case 'h':
		printf("HOW TO RUN\n");
		printf("I'll tell you when i figure it out.\n");
		printf("OPTIONS:\n");
		printf("-h Describe how the project should be run and then terminate.\n");
		printf("-n x Indicate the maximum total of child process oss will ever create. (Default 4)\n");
		printf("-s x Indicate the number of children allowed to exist in the system at the same time. (Default 2)\n");
		printf("-b B Start of the sequence of numbers to be tested for primality.\n");
		printf("-i I Increment between numbers that we test.\n");
		printf("-o filename Output file.\n");
		exit(0);
	case 'n':
		nValue = atoi(argv[optind]);
		break;
	case 's':
		sValue = atoi(argv[optind]);
		break;
	case 'b':
		bValue = atoi(argv[optind]);
		break;
	case 'i':
		iValue = atoi(argv[optind]);
		break;
	case 'o':
		strcpy(outputFile, argv[optind]);
		break;
	default:
		exit(0);
	}
}

/*---------------------------------------
	Test integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
int shmid = shmget(123, 1024, 0666|IPC_CREAT);

// error check for shmget
if (shmid < 0){
	printf("shmget error\n");
	exit(1);
}

// shmat to attach to shared memory
char *paddr  = (char*) shmat(shmid, NULL, 0);
int *pint = (int*)(paddr);
*pint = 10;
// detach from shared memory
shmdt(paddr);
/*---------------------------------------*/

/*---------------------------------------
	Make seconds integer for clock
		in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
int shmidCs = shmget(456, 1024, 0666 | IPC_CREAT);

// error check for shmget
if (shmidCs < 0) {
	printf("shmget error\n");
	exit(1);
}

// shmat to attach to shared memory
char* paddrCs = (char*)shmat(shmidCs, NULL, 0);
int* seconds = (int*)(paddrCs);
*seconds = 0;
// detach from shared memory
//shmdt(seconds);
/*---------------------------------------*/

/*---------------------------------------
	Make nano seconds integer for clock
		in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
int shmidCn = shmget(789, 1024, 0666 | IPC_CREAT);

// error check for shmget
if (shmidCn < 0) {
	printf("shmget error\n");
	exit(1);
}

// shmat to attach to shared memory
char* paddrCn = (char*)shmat(shmidCn, NULL, 0);
long int* nanoSeconds = (long int*)(paddrCn);
*nanoSeconds = 0;
// detach from shared memory
//shmdt(seconds);
/*---------------------------------------*/


/*--------------------------------------
	Shared memory flag array
---------------------------------------*/
// shmat to attach to shared memory
int* array;
// shmget returns an identifier in shmid
int shmidAr = shmget(0101, 1024, 0666 | IPC_CREAT);
// error check for shmget
if (shmidAr < 0) {
	printf("shmget error\n");
	exit(1);
}
array = (int*)shmat(shmidAr, NULL, 0);
int i;
for (i = 0; i < 21; i++) {
	array[i] = 0;
}
// detach from shared memory
shmdt(array);
/*-------------------------------------*/

int numberOfChildrenCreated = 0;
int childrenWorking = 0;
/* Test forking */
int pid;
/*--------------------------------------
			   THE LOOP
---------------------------------------*/
while (numberOfChildrenCreated <= nValue) {
	/*------------------------------------
				Real Clock
	-------------------------------------*/
	double totalTime;
	check = clock();
	totalTime = ((double)(check - start) / CLOCKS_PER_SEC);
	if (totalTime > 2)
	{
		printf("Process oss terminated: elapsed time greater than 2 seconds");
		exit(1);
	}
	/*------------------------------------
			  Simulated Clock
	-------------------------------------*/
	long int nanoSecondsHand = *nanoSeconds;
	nanoSecondsHand += 10000;
	*nanoSeconds = nanoSecondsHand;
	/*
	if (nanoSecondsHand > 1000000000) {
		int secondsHand = *seconds;
		secondsHand++;
		*seconds = secondsHand;

		nanoSecondsHand = (nanoSecondsHand - 1000000000);
		*nanoSeconds = nanoSecondsHand;
	}
	*/
	
	/*check to see if a process has termiinated
		using an array of shared memory flags*/	
	if (numberOfChildrenCreated >= sValue) {
		int* arrayC;
		int shmidArC = shmget(0101, 1024, 066);
		if (shmidArC == -1) {
			printf("error getting shmid\n");
			exit(1);
		}
		arrayC = (int*)shmat(shmidArC, NULL, 0);
		int i;
		for (i = 0; i < 21; i++) {
			if (arrayC[i] == 1) {
				childrenWorking--;
				arrayC[i] = 0;			
			}
		}				
	}
	/*If all children have died break loop*/
	if (numberOfChildrenCreated >= nValue && childrenWorking == 0)
		break;
	/*If max children have been created but some children are
		still working, then restart loop*/
	if (numberOfChildrenCreated >= nValue)
		continue;

	if (childrenWorking < sValue) {

		pid = fork();
		numberOfChildrenCreated++;
		childrenWorking++;

		if (pid < 0) {
			printf("There was a forking error.\n");
			exit(1);
		}//end if
		else if (pid == 0) {
			char* args[] = { "./prime", NULL };
			char* command = "./prime";
			char* arguments[2];
			char str[32];
			char primeStartingNumber[32];
			snprintf(str, sizeof(str), "%d", numberOfChildrenCreated);
			int recalc = (numberOfChildrenCreated - 1);
			snprintf(primeStartingNumber, sizeof(primeStartingNumber), "%d", (bValue + (iValue * recalc)));
			arguments[0] = str;
			arguments[1] = NULL;
			//int value = execvp(command, arguments);
			int value = execlp(command, str, primeStartingNumber, (char*)0);
			if (value < 0) {
				printf("Error with exec().\n");
				exit(1);
			}
		}		
	}
}
/*Deallocate shared memory*/
printf("Fork vim.\n");
shmctl(shmid, IPC_RMID, NULL);
shmctl(shmidAr, IPC_RMID, NULL);

/*test clock*/
printf("%d\n", *seconds);
printf("%d\n", *nanoSeconds);
shmdt(seconds);
shmdt(nanoSeconds);
shmctl(shmidCs, IPC_RMID, NULL);
shmctl(shmidCn, IPC_RMID, NULL);

return 0;
}
