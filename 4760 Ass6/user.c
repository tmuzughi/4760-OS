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

int memRI;
int pNum;
int pageTable[32];
int fruitLoops;
int memR;
int pageNum;



int main(int argc, char* argv[]) {
	/* initialize some values */
	srand(getpid());
	pNum = atoi(argv[0]);
	//TODO init PCB
	setPCB(getpid(), pNum,-1, -1, -1, -1, -1, -1, 0, 1);
	/* loopty loop */
	while (1) {
		/* get our memory reference address */
		int memR = (rand() % (32000 - 0 + 1)) + 0;

		/* pull page number */
		pageNum = (memR / 1000);

		/* read/write */
		int read = (rand() % (3 - 0 + 1)) + 0;
		if (read == 1) { //literacy 1 = write
			//hi
		}   
		else {
			read = 0;
		}
		/* update PCB */
		setPCB(getpid(), pNum, pageNum, memR, 99, 0, read, 0, pageNum, 0);

		/* wait for msg back */
		while (1) {
			int checkMe = checkCast(getpid(), pageNum);
			if (checkMe == 1 || checkMe == 0)
				break;
		}
		/* check for termination */
		fruitLoops++;
		if (fruitLoops > 1000) {
			break;
		}
	} 
	/* remove data from PCBT */
	setPCB(getpid(), -1, -1, -1, -1, -1, -1, -1, 0, 2);
	editCurrentNOP(-1);
	return 0;
}

