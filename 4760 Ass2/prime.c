#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

printf("I am prime.c\n");
//printf("%s\n", argv[0]);
//printf("%d\n", argc);

int shmid = shmget(123, 1024, 0666);
if (shmid == -1){
	printf("error getting shmid\n");
	exit(1);
}


int *pull = (int*) shmat(shmid, NULL, 0);

printf("%d\n", *pull);


printf("prime is done now\n");

return 0;

}
