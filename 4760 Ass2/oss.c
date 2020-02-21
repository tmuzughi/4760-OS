#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[]){

int options;
int nValue = 4;
int sValue = 2;
int bValue = 0;
int iValue = 0;
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


/* test argv input with options */
/*
int count = argc;
int i;

printf("%d\n", nValue);
printf("%d\n", sValue);
printf("%d\n", bValue);
printf("%d\n", iValue);
printf("%s\n", outputFile);



/* Test shared memory allocation/deallocation */

// ftok to generate unique key
//int uk = 1234;
//key_t key = uk;

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

//int *pull = (int*) shmat(shmid, NULL, 0);

//printf("%d\n", *pull);

//printf("%s\n", str);

// detach from shared memory
shmdt(paddr);
//shmdt(pull);

//printf("%s\n", str);

//remove shared memory segment
//shmctl(shmid, IPC_RMID, NULL);




int numberOfChildrenCreated = 0;
int childrenWorking = 0;
/* Test forking */


int pid = fork();

if (pid < 0){
	printf("There was a forking error.\n");
	exit(1);
}
else if (pid == 0){
	char *args[] = {"./prime", NULL};
	char *command = "./prime";
	char *arguments[2];
	char number_buff[32];
	sprintf(number_buff, "%d", shmid);
	arguments[0] = number_buff;
	arguments[1] = NULL;
	int value = execvp(command, arguments);
	if (value < 0){
		printf("Error with exec().\n");
		exit(1);
	} 
}
else {
	wait();
	printf("Child PID is %d.\n", pid);
  }

shmctl(shmid, IPC_RMID, NULL);

return 0;
}
