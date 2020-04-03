#ifndef OSS_H
#define OSS_H
#include <time.h>

typedef struct {

	int userPID;
	int vectorIndex;
	int timeCPU;
	int timeTotal;
	int timeDuringLastBurst;
	int priority;

} PCB;

typedef struct {

	int seconds;
	unsigned long int nanoSeconds;

} realFakeClock;

typedef struct {

	long msgType;
	char msgText[100];

} msgQueue;
#endif