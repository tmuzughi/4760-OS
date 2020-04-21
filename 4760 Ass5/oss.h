#ifndef OSS_H
#define OSS_H

typedef struct {
	int requested[20];
	int allocated;
	int allocatedTo[20];
	int available;
	int total;
} resourceDescriptor;

#endif