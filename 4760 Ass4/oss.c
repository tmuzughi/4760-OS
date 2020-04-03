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
#include "oss.h"
/*-------------------------------------------
				CONSTANTS
-------------------------------------------*/
#define maxTimeBetweenNewProcsNS 1000
#define maxTimeBetweenNewProcsSecs 1
#define maxBhatia 18
#define priorityPercentage 850

/*-------------------------------------------
			FUNCTION PROTOTYPES
-------------------------------------------*/
void createProcesses();
void stopNewProcesses();
int createProcessControlTable();
int createSharedSeconds();
void editSharedSeconds(int);
int fetchSharedSeconds();
int createSharedNanoSeconds();
void editSharedNanoSeconds(int);
unsigned long int fetchSharedNanoSeconds();
int allMyFriendsAreDead();
void checkDeaths();
int createMessage(int, int);
void getMessage(int);
int USPS();
void INThandler(int);

/*-------------------------------------------
			GLOBAL VARIABLES
-------------------------------------------*/
/* Trigger to stop new processes */
bool stopGenerating = false;
int msgsReceived = 0;
int tempIndex = 0;
int msgid;
int msgid1;
bool moreMail = true;
int timeInFuture = 0;
int pcbArrayID;
int deathArray;
int secondsID;
int nanoSecondsID;

/*-----------Create Bit 'Vector'-----------*/
int bitVector[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };
/*-------------Create Queues---------------*/
int queue0[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };
int queue1[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };
int queue2[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };
int queue3[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };
int queueBlocked[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0 };

int main(int argc, char* argv[]){
	
	/*-------------------------------------------
				 SIGNALS (TIMEOUT)
	-------------------------------------------*/
	struct itimerval it_val;
	if (signal(SIGALRM, (void (*)(int)) stopNewProcesses) == SIG_ERR) {
		perror("Unable to catch SIGALRM");
		exit(1);
	}
	it_val.it_value.tv_sec = 10;
	it_val.it_value.tv_usec = (10 * 1000) % 1000000;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	/* -------------------------
		 Set signal handler
	---------------------------*/
	signal(SIGINT, INThandler);

	/*------Create shared integers for Clock-----*/
	secondsID = createSharedSeconds();
	nanoSecondsID = createSharedNanoSeconds();

	/*---------------Create PCT------------------*/
	pcbArrayID = createProcessControlTable();

	/*-------Death Detection Array---------------*/
	deathArray = allMyFriendsAreDead();

	/* test create message */
	//createMessage(42069);
	//while (1);
	/*---------------Create Processes------------*/
	createProcesses();

	/* Handle mail for remaining processes */
	
	/*----------Remove Shared Memory-------------*/
	shmctl(pcbArrayID, IPC_RMID, NULL);
	shmctl(secondsID, IPC_RMID, NULL);
	shmctl(nanoSecondsID, IPC_RMID, NULL);
	shmctl(deathArray, IPC_RMID, NULL);
	msgctl(msgid, IPC_RMID, NULL);
	msgctl(msgid1, IPC_RMID, NULL);

return 0;
}

void createProcesses() {
	int numberOfProcesses = 0;
	int childrenWorking = 0;
	int isRealTime = 0;
	srand(time(0));
	timeInFuture = (rand() %
		(2 - 0 + 1)) + 0;
	int currentTime = 0;
	timeInFuture += currentTime;
	/* Test forking apporpriate amount of children */
	int pid;
	printf("OSS : %d\n", getpid());
	/*--------------------------------------------------
						 THE LOOP
	--------------------------------------------------*/
	bool yoda = true;
	while (numberOfProcesses < 100) {
		
		if (stopGenerating)
			break;
		/* Reset BitVector */
		checkDeaths();
		/* Generate random nanosecond number */
		srand(time(0));
		unsigned long int nanoNum = (rand() %
			(maxTimeBetweenNewProcsNS - 0 + 1)) + 0;
		/* Increment clock */
		editSharedSeconds(1);
		editSharedNanoSeconds(nanoNum);

		if (childrenWorking < 19) {
			
				numberOfProcesses++;
				childrenWorking++;



				pid = fork();

				if (pid != 0) {
					//createMessage(pid);
					//getMessage(pid);
					//int someNumber = USPS();
					tempIndex++;


				}
				/* If parent */
				if (pid != 0) {
					srand(pid);
					int priorityCheck = (rand() %
						(maxTimeBetweenNewProcsNS - 0 + 1)) + 0;
					int prioritySet = 1;
					if (priorityCheck <= priorityPercentage)
						prioritySet = 0; //realtime process
				/*-----------------------------------
						   INITIALIZE PCB
				-----------------------------------*/
				/* Assign user PID to bit vector */
					int userPID = (getpid() + numberOfProcesses);
					int i;
					for (i = 0; i < maxBhatia; i++) {
						if (bitVector[i] == 0) {
							bitVector[i] = pid;
							break;
						}
					}
					/* Initialize the PCB */
					PCB* pcbTable;
					/* We gon shmget us some memory! Yeehaw! */
					int shmidAr = shmget(0101, maxBhatia * sizeof(PCB), 0666);
					/* Check for errors */
					if (shmidAr == -1) {
						perror("error getting shmidAr\n");
						exit(1);
					}
					/* Attach our array to the shared memory */
					pcbTable = (PCB*)shmat(shmidAr, NULL, 0);
					/* Insert data into PCB */
					pcbTable[i].userPID = pid;
					pcbTable[i].vectorIndex = i;
					pcbTable[i].timeCPU = 0;
					pcbTable[i].timeTotal = 0;
					pcbTable[i].timeDuringLastBurst = 0;
					pcbTable[i].priority = prioritySet;

					/* Detach memory */
					shmdt(pcbTable);

					/* Set process into an initial queue */
					if (prioritySet == 1) {
						int i;
						for (i = 0; i < maxBhatia; i++) {
							if (queue0[i] == 0) {
								queue0[i] = pid;
								break;
							}
						}
					}
					else
					{
						srand(pid);
						int number = (rand() %
							(2 - 0 + 1)) + 0;
						if (number == 0) {
							int i;
							for (i = 0; i < maxBhatia; i++) {
								if (queue1[i] == 0) {
									queue1[i] = pid;
									break;
								}
							}
						}
						else if (number == 1) {
							int i;
							for (i = 0; i < maxBhatia; i++) {
								if (queue2[i] == 0) {
									queue2[i] = pid;
									break;
								}
							}
						}
						else
						{
							int i;
							for (i = 0; i < maxBhatia; i++) {
								if (queue3[i] == 0) {
									queue3[i] = pid;
									break;
								}
							}
						}

					}
					/*----------------------------------*/
				}
				if (pid < 0) {
					perror("There was a forking error.\n");
					exit(1);
				}
				else if (pid == 0) {
					//printf("User: %d\n", getpid());

					char* command = "./user";
					char isRealPass[32];

					snprintf(isRealPass, sizeof(isRealPass), "%d", isRealTime);

					int value = execlp(command, isRealPass, (char*)NULL);
					if (value < 0) {
						perror("Error with exec().\n");
						exit(1);
					}
				}
			
		}
		else {
		//int stuff = USPS();
			wait();
			childrenWorking--;
		}

		/*-----------------------------------
						USPS
		-----------------------------------*/
		
		//if (timeInFuture <= currentTime) {
			int arbitrary = USPS();
			//int randomTime = (rand() %
			//	(2 - 0 + 1)) + 0;
			//timeInFuture = (currentTime + randomTime);
		//}
		/* Set time for next iteration */
		currentTime = fetchSharedSeconds();
		//createMessage(bitVector[0]);
		//getMessage();
		//unsigned long int currentNanos = fetchSharedNanoSeconds();
		//currentTime += (currentNanos / 1000000000);
	} //end of while loop
	/*--------------------------------------------------*/
	int u;
	for (u = 0; u < 100; u++) {
		int numby = USPS();
		wait();
		//int someNumber = USPS();
	}
	int mySeconds = fetchSharedSeconds();
	unsigned long int myNanos = fetchSharedNanoSeconds();
	printf("Seconds: %d, NanoSeconds: %d\n", mySeconds, myNanos);
	int k;
	for (k = 0; k < 18; k++) {
		printf("Pid in BV: %d\n", bitVector[k]);
	}
	printf("%d\n", tempIndex);
	int i;
	for (i = 0; i < maxBhatia; i++) {
		printf("%d: ", queue0[i]);		
	}	
	printf("\n");
	for (i = 0; i < maxBhatia; i++) {
		printf("%d: ", queue1[i]);		
	}
	printf("\n");
	for (i = 0; i < maxBhatia; i++) {
		printf("%d: ", queue2[i]);		
	}
	printf("\n");
	for (i = 0; i < maxBhatia; i++) {
		printf("%d: ", queue3[i]);	
	}
	printf("\n");
}

void stopNewProcesses() {
	stopGenerating = true;
}

int createProcessControlTable() {
	PCB* pcbArray;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(0101, maxBhatia * sizeof(PCB), 0666 | IPC_CREAT);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error on shmidAr1\n");
		exit(1);
	}
	/* Attach shared memory */
	pcbArray = (PCB*)shmat(shmidAr1, NULL, 0);

	/* Instantiate PCBs with arbitrary data */
	int i;
	for (i = 0; i < 18; i++) {
		pcbArray[i].userPID = 0;
	}

	/* Detach from shared memory */
	shmdt(pcbArray);

	return shmidAr1;
}

int createSharedSeconds() {
	/*---------------------------------------
		Make seconds integer for clock
			in shared memory
		----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 0666 | IPC_CREAT);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds = (unsigned long int*)(paddrS);
	*seconds = 0;

	// detach from shared memory
	shmdt(seconds);

	return shmidS;
}

void editSharedSeconds(int increment) {
	/*---------------------------------------
		Edit seconds integer for clock
			in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds = (unsigned long int*)(paddrS);
	*seconds += increment;

	// detach from shared memory
	shmdt(seconds);

}

int fetchSharedSeconds() {
	/*---------------------------------------
		Fetch seconds integer for clock
			in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidS = shmget(678, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrS = (char*)shmat(shmidS, NULL, 0);
	unsigned long int* seconds;
	seconds = (unsigned long int*)(paddrS);
	//printf("The number of seconds is %d\n", *seconds);
	int secondsPass = *seconds;
	// detach from shared memory
	shmdt(seconds);

	return secondsPass;
}

int createSharedNanoSeconds() {
	/*---------------------------------------
	Make nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 0666 | IPC_CREAT);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds = (unsigned long int*)(paddrNS);
	*nanoSeconds = 0;

	// detach from shared memory
	shmdt(nanoSeconds);
	
	return shmidNS;
}

void editSharedNanoSeconds(increment) {
	/*---------------------------------------
	Edit nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds = (unsigned long int*)(paddrNS);
	*nanoSeconds += increment;

	// detach from shared memory
	shmdt(nanoSeconds);
}

unsigned long int fetchSharedNanoSeconds() {
	/*---------------------------------------
	Fetch nano seconds integer for clock
		in shared memory
	----------------------------------------*/
	// shmget returns an identifier in shmid
	int shmidNS = shmget(789, sizeof(unsigned long int), 066);

	// error check for shmget
	if (shmidNS < 0) {
		perror("shmget error\n");
		exit(1);
	}

	// shmat to attach to shared memory
	char* paddrNS = (char*)shmat(shmidNS, NULL, 0);
	unsigned long int* nanoSeconds;
	nanoSeconds = (unsigned long int*)(paddrNS);
	//printf("The number of nano-seconds is %d\n", *nanoSeconds);
	unsigned long int nanoSecondsPass = *nanoSeconds;

	// detach from shared memory
	shmdt(nanoSeconds);

	return nanoSecondsPass;
}

int allMyFriendsAreDead() {
	/*---------------------------------------
	Set up shared memory array of size 'maxBhatia'
	----------------------------------------*/
	int* array1;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(511, 100 * sizeof(int), 0666 | IPC_CREAT);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array1 = (int*)shmat(shmidAr1, NULL, 0);
	/* Put integers from file into local array */
	int i = 0;
	for (i = 0; i < 100; i++) {
		array1[i] = 0;
	}
	/* Detach our array from shared memory */
	shmdt(array1);
	/*-------------------------------------*/

	return shmidAr1;
}

void checkDeaths() {
	/*---------------------------------------
	Set up shared memory array of size 'maxBhatia'
	----------------------------------------*/
	int* array1;
	/* We gon shmget us some memory! Yeehaw! */
	int shmidAr1 = shmget(511, 100 * sizeof(int), 066);
	/* Check for errors */
	if (shmidAr1 < 0) {
		perror("shmget error\n");
		exit(1);
	}
	/* Attach our array to the shared memory */
	array1 = (int*)shmat(shmidAr1, NULL, 0);
	/* Put integers from file into local array */
	int i = 0;
	int number;
	for (i = 0; i < 100; i++) {
		
		if (array1[i] != 0) {
			
			int k;
			for (k = 0; k < maxBhatia; k++) {
				if (bitVector[k] == array1[i]) {
					number = bitVector[k];
					bitVector[k] = 0;					
					break;
				}
			}
		}
	}
	//remove from queues
	for (i = 0; i < maxBhatia; i++) {
		if (queue0[i] == number) {
			queue0[i] = 0;
			break;
		}
	}
	
	for (i = 0; i < maxBhatia; i++) {
		if (queue1[i] == number) {
			queue1[i] = 0;
			break;
		}
	}
	
	for (i = 0; i < maxBhatia; i++) {
		if (queue2[i] == number) {
			queue2[i] = 0;
			break;
		}
	}
	
	for (i = 0; i < maxBhatia; i++) {
		if (queue3[i] == number) {
			queue3[i] = 0;
			break;
		}
	}
	/* Detach our array from shared memory */
	shmdt(array1);
	/*-------------------------------------*/
}

int createMessage(pid, queue) {
	msgid;
	msgid = msgget(501, 0666 | IPC_CREAT);
	msgQueue sending;
	
	sending.msgType = pid; //<--the intended user pid
	char text[100];
	sprintf(text, "%d", queue);
	strcpy(sending.msgText, text);
	msgsnd(msgid, &sending, sizeof(sending), IPC_NOWAIT);
}

void getMessage(pid) {
	msgid1;
	msgQueue receiving;
	msgid1 = msgget(502, 066);
	msgrcv(msgid1, &receiving, sizeof(receiving), pid, 0);

	printf("Message received from user %d.\n", receiving.msgType);
	//msgctl(msgid, IPC_RMID, NULL);

}

int USPS() {
	/* Increment clock */
	srand(time(0));
	int nanosToAdd = (rand() %
		( 10000 - 100 + 1)) + 100;
	editSharedNanoSeconds(nanosToAdd);
	int i;
	for (i = 0; i < maxBhatia; i++) {
		if (queue0[i] != 0) {
			createMessage(queue0[i], 0);
			getMessage(queue0[i]);
			queue0[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < maxBhatia; i++) {
		if (queue1[i] != 0) {
			createMessage(queue1[i], 1);
			getMessage(queue1[i]);
			queue1[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < maxBhatia; i++) {
		if (queue2[i] != 0) {
			createMessage(queue2[i], 2);
			getMessage(queue2[i]);
			queue2[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < maxBhatia; i++) {
		if (queue3[i] != 0) {
			createMessage(queue3[i], 3);
			getMessage(queue3[i]);
			queue3[i] = 0;
			return 0;
		}
	}
	moreMail = false;
	return 0;
}

void INThandler(int sig)
{

	signal(sig, SIG_IGN);
	shmctl(pcbArrayID, IPC_RMID, NULL);
	shmctl(secondsID, IPC_RMID, NULL);
	shmctl(nanoSecondsID, IPC_RMID, NULL);
	shmctl(deathArray, IPC_RMID, NULL);
	msgctl(msgid, IPC_RMID, NULL);
	msgctl(msgid1, IPC_RMID, NULL);

	printf("\nCtrl + C detected. Master process shutting down. Beep. Boop.\n");
	exit(0);

}