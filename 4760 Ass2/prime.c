#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/prctl.h>
#include <signal.h>

void terminator(long int *now, long int save, int child) {
	//printf("calculation is %d - %d which is %d\n", *now, save, (*now - save));
	long int check = (*now - save);
	if (check > 1000000) {
		printf("Child %d terminated.\n", child);
		/*set the flag array*/
		int* array;
		int shmidAr = shmget(0101, 1024, 0666);
		if (shmidAr == -1) {
			perror("error getting shmid\n");
			exit(1);
		}
		array = (int*)shmat(shmidAr, NULL, 0);
		array[child] = 1;
		/*get dumn ole nValue*/
		// shmget returns an identifier in shmid
		int shmid = shmget(123, 1024, 0666);
		// error check for shmget
		if (shmid < 0) {
			perror("shmget error\n");
			exit(1);
		}
		// shmat to attach to shared memory
		
		int *pint = (int*)shmat(shmid, NULL, 0);
		/*set the 'finish' array*/
		int* array1;
		int shmidAr1 = shmget(0202, *pint * sizeof(int), 0666);
		if (shmidAr1 == -1) {
			perror("error getting shmid\n");
			exit(1);
		}
		array1 = (int*)shmat(shmidAr1, NULL, 0);
		array1[child] = -1;
		shmdt(array);
		shmdt(array1);
		shmdt(pint);
		exit(1);
	}
}

int main(int argc, char* argv[]){
	/*following should terminate self if parent terminates*/
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	/*Child process looks into shared memory and logs time 
	of creating in simulated time*/
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

	/*check for primality*/
	int n, i, flag = 1;

	n = atoi(argv[1]);

	// Iterate from 2 to n/2 
	for (i = 2; i <= sqrt(n); i++) {
		terminator(now, save, atoi(argv[0]));
		// If n is divisible by any number between 
		// 2 and n/2, it is not prime 
		if (n % i == 0) {
			flag = 0;
			break;
		}
	}
	if (flag == 1) {
		//printf("%d is a prime number\n", n);
		int* array;
		int shmidAr = shmget(0101, 1024, 066);
		if (shmidAr == -1) {
			perror("error getting shmid1\n");
			exit(1);
		}
		array = (int*)shmat(shmidAr, NULL, 0);
		array[atoi(argv[0])] = 1;
		/*detach shared memory*/
		shmdt(array);
		//shmdt(pull);
		shmdt(now);

		/*get dumn ole nValue*/
		// shmget returns an identifier in shmid
		int shmid = shmget(123, 1024, 0666);
		// error check for shmget
		if (shmid < 0) {
			perror("shmget error\n");
			exit(1);
		}
		// shmat to attach to shared memory

		int* pint = (int*)shmat(shmid, NULL, 0);
		/*set the 'finish' array*/
		int* array1;
		int shmidAr1 = shmget(0202, *pint * sizeof(int), 0666);
		if (shmidAr1 == -1) {
			perror("error getting shmid2\n");
			exit(1);
		}
		array1 = (int*)shmat(shmidAr1, NULL, 0);
		array1[atoi(argv[0])] = n;
		shmdt(array1);

		exit(0);
	}
	else {
		//printf("%d is not a prime number\n", n);
		int* array;
		int shmidAr = shmget(0101, 1024, 066);
		if (shmidAr == -1) {
			perror("error getting shmid0\n");
			exit(1);
		}
		array = (int*)shmat(shmidAr, NULL, 0);
		array[atoi(argv[0])] = 1;
		/*detach shared memory*/
		shmdt(array);
		//shmdt(pull);
		shmdt(now);

		/*get dumn ole nValue*/
		// shmget returns an identifier in shmid
		int shmid = shmget(123, 1024, 0666);
		// error check for shmget
		if (shmid < 0) {
			perror("shmget error\n");
			exit(1);
		}
		// shmat to attach to shared memory

		int* pint = (int*)shmat(shmid, NULL, 0);
		/*set the 'finish' array*/
		int* array1;
		int shmidAr1 = shmget(0202, *pint * sizeof(int), 0666);
		if (shmidAr1 == -1) {
			perror("error getting shmid5\n");
			exit(1);
		}
		array1 = (int*)shmat(shmidAr1, NULL, 0);
		array1[atoi(argv[0])] = -n;
		shmdt(array1);

		exit(0);
	}


return 0;

}
