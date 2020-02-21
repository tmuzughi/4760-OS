#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

*/

/* Test shared memory allocation/deallocation */
/*
// ftok to generate unique key
//key_t key = ftok("shmfile", 65);

// shmget returns an identifier in shmid
int shmid = shmget(IPC_PRIVATE, 1024, 0666|IPC_CREAT);

// error check for shmget
if (shmid < 0){
	printf("shmget error\n");
	exit(1);
}

// shmat to attach to shared memory
char *str = (char*) shmat(shmid, (void*) 0, 0);

str = "dog";

printf("%s\n", str);

// detach from shared memory
shmdt(str);

//printf("%s\n", str);

//remove shared memory segment
shmctl(shmid, IPC_RMID, NULL);

//try to pull from shared memory (should fail)
printf("%s", str);

*/

/* Test forking */

int pid = fork();

if (pid < 0){
	printf("There was a forking error.\n");
	exit(1);
}
else if (pid == 0){
	char *args[] = {"./prime", NULL};
	execvp(args[0], args); 
}
else{
	wait();
	printf("Child PID is %d.\n", pid);
}









return 0;
}
