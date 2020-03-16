***Program Goal***

1. Partition the n integers into n/2 pairs. Use n/2 processes to add together each pair of 
integers resulting into n/2 integers. Repeat the method on the n/2 integers and continue until
 the ﬁnal result is obtained. (This is a binary tree algorithm.)
2. Divide the n integers into n/logn groups of logn numbers each. Use n/logn processes, each 
adding the numbers in one group sequentially. Then add the n/logn results using method 1.
3. Log results to file 'adder_log' using semaphores to protect the critical section

***Options***

For the grader's convenience, I have added the following options:
-h Describe how to run, options, then terminate.
-r Remove wait time surrounding critical section access.
-a # Adjust the time (in seconds) allowed for computation. Default is 100.

***Input File***

If you do not wish to use the provided input file and you do not wish to use the 'numbers' 
program to create an input file for testing, then you will need to be sure your input
file is named 'inputFile.txt".

If you would like to use the program 'numbers' to generate in input file, do the following: 
compile the program with 'gcc -o numbers numbers.c', then run the program with either
'numbers' or 'numbers #' (e.g. 'numbers 32'). If run without an integer argument, the default
number of integers provided will be 64. Be sure that the number provided is a power
of 2, 'numbers' will not check that this is so and 'master' may give unexpected results in
the event of an input file with total numbers not equal to a power of 2.

***Git log***

3/10/20
* Created git repo
* Created input file generator ('numbers')

3/11/20
* Added real time clock 
* Created function to check for 'timeout'
* Created function to determine number of integers in file
* Created function to fetch integers from file and put them into shared memory
* Created function to test pulling integers from shared memory

3/12/20
* Created base bin_adder.c
* Created function to fork off priliminary number of children
* Added handler for 'Ctrl + C'

3/13/20
* Created Makefile
* Edited master forking function to send proper data to children
* Created 'control array' in shared memory
  - used for tracking progress of children during summation process
* Created function to fetch integers from shared memory in bin_adder
* Created function to fetch control integers from shared memory in bin_adder
* Created summation function in bin_adder
* Created process wherein 'Master child' (repeating) forks additional children
  - First child created acts as new 'master' 
  - Forks necessary children using control array to track progress
  - New 'first child' repeats process until all calculations have been completed
  - Master children adjust control array and wait for children to die before terminating

3/14/20
* Created semaphore from master
* Created function to use semaphore in bin_adder
* Created function to update adder_log file
* Created fork process for first step of log method

3/15/20
* Created function for log summation
* Looped process back to first forking function
* Lots of debugging

3/16/20
* More debugging
* Testing
* Even more debugging
* Added options for grader
* Cleaned up some unneccessary code
* Updated README summary information/instructions