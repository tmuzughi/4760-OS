#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>

void INThandler(int);

int x;


int main(int argc, char* argv[]){

	signal(SIGINT, INThandler);
	

	
	/*Start 'real time' clock*/
	clock_t start, check;
	start = clock();

int options;
int nValue = 4;
int sValue = 2;
int bValue = 0;
int iValue = 5;
char outputFile[100] = "output.log";

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
x = nValue;
/*Create/clear/open output file*/
FILE* file;
char* mode = "w";
file = fopen(outputFile, mode);
if (file == NULL)
perror("Could not open file.");
fprintf(file, "%s", "");
fclose(file);
mode = "a";
file = fopen(outputFile, mode);
/*Keep s value at 20 or lower*/
if (sValue > 20)
	sValue = 20;
/*---------------------------------------
	Test integer in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
int shmid = shmget(123, 1024, 0666|IPC_CREAT);

// error check for shmget
if (shmid < 0){
	perror("shmget error\n");
	exit(1);
}

// shmat to attach to shared memory
char *paddr  = (char*) shmat(shmid, NULL, 0);
int *pint = (int*)(paddr);
*pint = nValue;
// detach from shared memory
shmdt(paddr);
shmdt(pint);
/*---------------------------------------*/

/*---------------------------------------
	Make seconds integer for clock
		in shared memory
----------------------------------------*/
// shmget returns an identifier in shmid
int shmidCs = shmget(456, 1024, 0666 | IPC_CREAT);

// error check for shmget
if (shmidCs < 0) {
	perror("shmget error\n");
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
	perror("shmget error\n");
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
	Shared memory array for primality
---------------------------------------*/
// shmat to attach to shared memory
int* array1;
// shmget returns an identifier in shmid
int shmidAr1 = shmget(0202, nValue * sizeof(int), 0666 | IPC_CREAT);
// error check for shmget
if (shmidAr1 < 0) {
	perror("shmget error\n");
	exit(1);
}
array1 = (int*)shmat(shmidAr1, NULL, 0);
int i;
for (i = 0; i < (nValue + 1); i++) {
	array1[i] = 0;
}
// detach from shared memory
shmdt(array1);
/*-------------------------------------*/

/*--------------------------------------
	Shared memory flag array
---------------------------------------*/
// shmat to attach to shared memory
int* array;
// shmget returns an identifier in shmid
int shmidAr = shmget(0101, 1024, 0666 | IPC_CREAT);
// error check for shmget
if (shmidAr < 0) {
	perror("shmget error\n");
	exit(1);
}
array = (int*)shmat(shmidAr, NULL, 0);

for (i = 0; i < (nValue + 1); i++) {
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
			perror("error getting shmid\n");
			exit(1);
		}
		arrayC = (int*)shmat(shmidArC, NULL, 0);
		int i;
		for (i = 0; i < (nValue + 1); i++) {
			if (arrayC[i] == 1) {
				childrenWorking--;
				arrayC[i] = 0;

				/*get starting time in nanoseconds*/
				int shmidCn = shmget(789, 1024, 0666);
				if (shmidCn == -1) {
					perror("error getting shmid\n");
					exit(1);
				}
				long int* now = (long int*)shmat(shmidCn, NULL, SHM_RDONLY);
				/*the simulated start time in nanoseconds is
				store in save*/
				long int save = *now;
				/*output to file time when x child was created*/
				fprintf(file, "Child %d terminated at %d nanoseconds.\n", i, save);
				shmdt(now);
				break;
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
		/*get starting time in nanoseconds*/
		int shmidCn = shmget(789, 1024, 0666);
		if (shmidCn == -1) {
			perror("error getting shmid\n");
			exit(1);
		}
		long int* now = (long int*)shmat(shmidCn, NULL, SHM_RDONLY);
		/*the simulated start time in nanoseconds is
		store in save*/
		long int save = *now;
		/*output to file time when x child was created*/
		fprintf(file, "Child %d created at %d nanoseconds.\n", (numberOfChildrenCreated + 1), save);
		shmdt(now);
		numberOfChildrenCreated++;
		childrenWorking++;

		if (pid < 0) {
			perror("There was a forking error.\n");
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
				perror("Error with exec().\n");
				exit(1);
			}
		}		
	}
}
int* arrayf;
int shmidArf = shmget(0202, *pint * sizeof(int), 0666);
if (shmidArf == -1) {
	perror("error getting shmid2\n");
	exit(1);
}
arrayf = (int*)shmat(shmidArf, NULL, 0);
int j;
fprintf(file, "Prime numbers: ");
for (j = 1; j < (nValue + 1); j++) {
	if (arrayf[j] > 0) {
		fprintf(file, "%d ", arrayf[j]);
	}
}
fprintf(file, "\n");
fprintf(file, "Non prime numbers: ");
for (j = 1; j < (nValue + 1); j++) {
	if (arrayf[j] <= 0) {
		fprintf(file, "%d ", arrayf[j]);
	}
}
fprintf(file, "\n");
fprintf(file, "Numbers unable to finish: ");
for (j = 1; j < (nValue + 1); j++) {
	if (arrayf[j] == -1) {
		fprintf(file, "%d ", (bValue + (sValue * j)));
	}
}
fprintf(file, "\n");
/*get starting time in nanoseconds*/
int shmidCf = shmget(789, 1024, 0666);
if (shmidCf == -1) {
	perror("error getting shmid\n");
	exit(1);
}
long int* now1 = (long int*)shmat(shmidCf, NULL, SHM_RDONLY);
/*the simulated start time in nanoseconds is
store in save*/
long int save1 = *now1;
fprintf(file, "Parent process terminated at %d nanoseconds.", save1);
/*Deallocate shared memory*/
//printf("Fork vim.\n");
shmctl(shmid, IPC_RMID, NULL);
shmctl(shmidAr, IPC_RMID, NULL);

shmdt(seconds);
shmdt(nanoSeconds);
shmctl(shmidCs, IPC_RMID, NULL);
shmctl(shmidCn, IPC_RMID, NULL);
shmctl(shmidAr1, IPC_RMID, NULL);
shmctl(shmidCf, IPC_RMID, NULL);
fclose(file);
return 0;
}

void  INThandler(int sig)
{
	FILE* file;
	signal(sig, SIG_IGN);
	char *mode = "a";
	file = fopen("output.log", mode);
	/*get starting time in nanoseconds*/
	int shmidCf = shmget(789, 1024, 0666);
	if (shmidCf == -1) {
		printf("error getting shmid\n");
		exit(1);
	}
	long int* now1 = (long int*)shmat(shmidCf, NULL, SHM_RDONLY);
	/*the simulated start time in nanoseconds is
	store in save*/
	long int save1 = *now1;
	fprintf(file, "Parent process terminated at %d nanoseconds.", save1);
	fclose(file);
	int shmid = shmget(123, 1024, 0666);
	int shmidAr = shmget(0101, 1024, 0666);
	int shmidCn = shmget(789, 1024, 0666);
	int shmidCs = shmget(456, 1024, 0666);
	int shmidAr1 = shmget(0202, x * sizeof(int), 0666);
	/*Deallocate shared memory*/
	shmctl(shmid, IPC_RMID, NULL);
	shmctl(shmidAr, IPC_RMID, NULL);
	shmctl(shmidCs, IPC_RMID, NULL);
	shmctl(shmidCn, IPC_RMID, NULL);
	shmctl(shmidAr1, IPC_RMID, NULL);
	shmctl(shmidCf, IPC_RMID, NULL);
	exit(0);
	
}