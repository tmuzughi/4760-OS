#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN_NUM 64
#define MAX_NUM 100
#define MAX_BHATIA 256

int main(int argc, char* argv[]) {
	/*Program creates input file for another program

	input file should have...
	*** at least 64 integers total
	*** total number of integers should be power of 2
	*** integers should be in range [0-256]
	*/

	/* FOR NOW ONLY 64 TOTAL IS OPTION WILL ENHANCE LATER */

	/* Initialization, should only be called once */
	srand(time(NULL));	 

	/* Open file */
	FILE* file;
	char* mode = "w";
	file = fopen("inputFile.txt", mode);

	/* File error check */
	if (file == NULL)
		perror("Could not open file.");

	/* Fill inputFile */
	int i;
	for (i = 0; i < MIN_NUM; i++) {
		/* Get a random number within the range */
		int r = -1;
		while (r < 0 || r > MAX_BHATIA) {
			r = rand();
		}
		fprintf(file, "%d\n", r);
	}
	/* Close file*/
	fclose(file);

	return 0;
}