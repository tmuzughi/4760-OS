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
void forkOneProcess();


/*----------------------------------------
				GLOBALS
----------------------------------------*/
int currentNOP = 0; //Number of Processes that exist now
int maxCurrentNOP = maxBhatia; //Maximum Simultaneous Number Of Processes
int totalNOP = 0; //How many processes exist cumulative
int maxTotalNOP = 10; //Total processes we will create
int memoryAccessType = 0;
FILE* file;
int ft[256];

int main(int argc, char* argv[]) {
	/* OPTIONS */
	int options;
	while ((options = getopt(argc, argv, "hm:")) != -1) {
		 
		switch (options) {
		case 'h':
			printf("HOW TO RUN\n");
			printf("I'll tell you when i figure it out.\n");
			printf("OPTIONS:\n");
			printf("-h Describe how the project should be run, options, and then terminate.\n");
			printf("-m x Specify memory access type, 0 for simple, 1 for weighted : (0 by default)\n");
			exit(0);
		case 'm':	
			if (atoi(optarg) == 0 || atoi(optarg) == 1)
			memoryAccessType = atoi(optarg);
			break;	
		default:
			exit(0);
		}
	}
	/* Create/wipe log file */
	file = fopen("output_log", "w");
	fclose(file);

	/* Ctrl + C signal handler */
	signal(SIGINT, INThandler);

	int m;
	for (m = 0; m < 256; m++) {
		ft[m] = 0;
	}

	/* Make semaphore */
	int sem = makeSemaphore();

	/*---------------Create PCT------------------*/
	int pcbArrayID = createProcessControlTable();

	/* Create shared nano seconds */
	int sharedNanoID = createSharedNanoSeconds();

	/* Create current NOP ID */
	int currentNOPID = createCurrentNOP();

	/* Create request */
	int requestID = createRequest();

	/* init */
	remplir();
	 
	/*---------------------------------
					loop
	---------------------------------*/
	unsigned long int timeToCreate = 0;
	 
	int times = 1;
	srand(getpid());
	while (1) {
		 
		/* check request */
		 
		processRequests(ft);
		
		/* Increment the nanoseconds */
		semBegin();
		editSharedNanoSeconds(1000000);
		semEnd();

		/* Get shared nanoseconds */
		unsigned long int currentNanos = fetchSharedNanoSeconds();

		currentNOP = fetchCurrentNOP();
		if (currentNOP < 18) {
			if (totalNOP < maxTotalNOP) {


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
		}
		else
		{		
			while (wait(NULL) > 0) {

			}		
		}
		if (totalNOP >= maxTotalNOP) {
			if (currentNOP == 0)
				break;
		}
	}
	
	/* "Wait" for all remaining user processes to terminate */
	int k;
	int* check;
	for (k = 0; k < 99; k++) {
		check = checkRequest();
		wait(NULL);
	}
	if (check[0] != -1) {
		printf("P%d requested %d\n", *check, *(check + 1));
	}
	printMemLayout();
	/*----------------------------------*/
	/* Remove shared memory */
	shmctl(sharedNanoID, IPC_RMID, NULL);
	shmctl(pcbArrayID, IPC_RMID, NULL);
	shmctl(currentNOPID, IPC_RMID, NULL);
	shmctl(requestID, IPC_RMID, NULL);
	if (semctl(sem, 0, IPC_RMID, 0) != 0) {
		fprintf(stderr, "Couldn't remove the semahpore!\n");
		exit(1);
	}

	return 0;
}

void forkOneProcess() {
	//printf("Current # of processes are %d\n", currentNOP);
	int pid = fork();
	if (pid != 0) {
	
	}
	if (pid < 0) {
		//printf("reforking\n");
		wait(NULL);
		int pid = fork();
		if (pid < 0) {
			perror("There was a forking error.\n");
			printf("Current processes %d\n", currentNOP);
			printf("Total processes %d\n", totalNOP);
			int shmid = shmget(77789, 1024, 066);		
			int shmidNS = shmget(789, sizeof(unsigned long int), 066);
			int requestID = shmget(1337, 2 * sizeof(int), 066);
			int pcbArrayID = shmget(42096, 18 * sizeof(PCB), 066);
			shmctl(shmid, IPC_RMID, NULL);	
			shmctl(pcbArrayID, IPC_RMID, NULL);
			shmctl(shmidNS, IPC_RMID, NULL);
			shmctl(requestID, IPC_RMID, NULL);
			exit(1);
		} 
		else if (pid == 0) {
			char* command = "./user";
			char isRealPass[32];
			snprintf(isRealPass, sizeof(isRealPass), "%d", totalNOP);
			char isSchemePass[32];
			snprintf(isSchemePass, sizeof(isSchemePass), "%d", memoryAccessType);

			int value = execlp(command, isRealPass, isSchemePass, (char*)NULL);
			if (value < 0) {
				perror("Error with exec().\n");
				exit(1);
			}
		}
	}   
	else if (pid == 0) {
		//printf("goodbye, cruel world\n");

		char* command = "./user";
		char isRealPass[32];
		snprintf(isRealPass, sizeof(isRealPass), "%d", totalNOP);
		char isSchemePass[32];
		snprintf(isSchemePass, sizeof(isSchemePass), "%d", memoryAccessType);

		int value = execlp(command, isRealPass, isSchemePass, (char*)NULL);
		if (value < 0) {
			perror("Error with exec().\n");
			exit(1);
		}
	}
}

void remplir() {
	
		
} 


void processRequests() {
	PCB* pcbArray;
	unsigned long int nanos;
	semBegin();
	FILE* file;
	file = fopen("output_log", "a");
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(42096, 18 * sizeof(PCB), 066);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on PCBT2\n");
		exit(1);
	}
	/* Attach shared memory */
	pcbArray = (PCB*)shmat(shmidAr1, NULL, 0);

	int i;

	for (i = 0; i < 18; i++) {
		if (pcbArray[i].cast > -1) {
			//found request
			/* set some variables */
			int index = pcbArray[i].index;
			int ptIndex = pcbArray[i].ptIndex;
			int address = pcbArray[i].pageTable[ptIndex].address;
			int literacy = pcbArray[i].pageTable[ptIndex].literacy;
			int isValid = pcbArray[i].pageTable[ptIndex].isValid;
			int frame = pcbArray[i].pageTable[ptIndex].frame;

			if (literacy == 0) {
				nanos = fetchSharedNanoSeconds();
				fprintf(file, "P%d requesting read of address %d at time 0:%d\n",
					index, address, nanos);
			}
			else {
				nanos = fetchSharedNanoSeconds();
				fprintf(file, "P%d requesting write of address %d at time 0:%d\n",
					index, address, nanos);
			}
			if (isValid == 1) {
				nanos = fetchSharedNanoSeconds();
				fprintf(file, "Address %d in frame %d, giving data to P%d at time 0:%d\n",
					address, frame, index, nanos);
			}
			else {
				fprintf(file, "Address %d is not in a frame, pagefault\n", address);
				/* check for free memory */
				int m;
				bool freeMem = false;
				for (m = 0; m < 256; m++) {
					if (ft[m] == 0) {
						freeMem = true;
						break;
					} 
				}
				if (freeMem == true) {
					/* we found mem */
					fprintf(file, "Frame %d clear, assigning to P%d page %d\n",
						m, index, ptIndex);
					//fprintf(file, "im m %d\n", m);
					ft[m] = 1;
					if (literacy == 1) {
						fprintf(file, "Indicating to P%d that write has happened to address %d\n",
							index, address);
					}
					else {
						nanos = fetchSharedNanoSeconds();
						fprintf(file, "Address %d in frame %d, giving data to P%d at time 0:%d\n",
							address, m, index, nanos);
					}
					//fprintf(file, "im m2 %d\n", m);
					pcbArray[i].pageTable[ptIndex].frame = m;
					//fprintf(file, "just set frame %d, thats frame %d\n", m, pcbArray[i].pageTable[ptIndex].frame);
				}
				else {
					/* we didnt find mem */
					srand(pcbArray[i].pid);
					int r = (rand() % (256 - 0 + 1)) + 0;
					fprintf(file, "Clearing frame %d and swapping in P%d page %d\n",
						r, index, ptIndex);
					pcbArray[i].pageTable[ptIndex].frame = r;
					fprintf(file, "Dirty bit of frame %d set, adding additional time to the clock\n", r);
					pcbArray[i].pageTable[ptIndex].dirtyBit = 1;
					if (literacy == 1)
						fprintf(file, "Indicating to P%d that write has happened to address %d\n",
							index, address);
					else {
						nanos = fetchSharedNanoSeconds();
						fprintf(file, "Address %d in frame %d, giving data to P%d at time 0:%d\n",
							address, r, index, nanos);
					}

				}
				pcbArray[i].pageTable[ptIndex].isValid = 1;
			}
			pcbArray[i].cast = 0;
		}
	}


	/* Detach from shared memory */
	shmdt(pcbArray);
	fclose(file);
	semEnd();
}