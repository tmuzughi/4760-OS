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
#include <sys/prctl.h>
#include <signal.h>
#include "oss.h"
#define baseTime 1000000

void dontWeepForMe();
void getMessage();

int iRun = 0;
bool waiting = true;

int main(int argc, char* argv[]){
	/*following should terminate self if parent terminates*/
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	
	
	while (waiting) {
		getMessage();
	}
		createMessage();
		//printf("%d\n", iRun);
	
	
	//printf(" : My PID is %d\n", getpid());
	dontWeepForMe();
	
return 0;
}

void dontWeepForMe() {
	
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
	int i;
	for (i = 0; i < 100; i++) {
		
		if (array1[i] == 0) {
			array1[i] = getpid();
			break;
		}
	}
	/* Detach our array from shared memory */
	shmdt(array1);
	/*-------------------------------------*/
}

int createMessage() {
	int* msgid;
	msgid = msgget(502, 0666 | IPC_CREAT);
	msgQueue sending;

	sending.msgType = getpid();
	strcpy(sending.msgText, "stuff about me");
	msgsnd(msgid, &sending, sizeof(sending), IPC_NOWAIT);
}

void getMessage() {
	//printf("we in this bitch\n");
	int msgid;
	msgQueue receiving;
	msgid = msgget(501, 066);
	msgrcv(msgid, &receiving, sizeof(receiving), getpid(),IPC_NOWAIT);
	if (receiving.msgType = getpid()) {
		waiting = false;

		printf("My pid is %d and message says %d in Q %d\n", getpid(), receiving.msgType, atoi(receiving.msgText));
	}
	
	iRun = 1;
	//createMessage();
	//msgctl(msgid, IPC_RMID, NULL);
	
}
