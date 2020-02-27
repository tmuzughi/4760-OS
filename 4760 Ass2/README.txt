# 4760-OS

How to run:
./oss

To see a list of options:
./oss -h

Git log:
2/19/20
*Created .c files
*Created Makefile
*Added and tested options

2/20/20
*Tested allocating and deallocating shared memory
*Tested forking and exec

2/21/20
*Successfully created child process with fork, execvp
*Placed integer in shared memory from parent process
*Pulled integer from shared memory with child process
*Created main loop which fork/execs appropriate amount (max) of children
*Modified loop to 'wait' (not 'wait()') when 'number
	 of allowed, simultaneous' children is exceeded
*Passed child id (which number child, not PID) to child
*Created shared memory array for flags to determine
	what processes had finished
*Created loop which checks for termination
- child processes signal the flag at their position
	in the shared memory array
- parent process can then note that child's termination,
	handle information apropriately, and lower flag

2/24/20
*Added clock including naonseconds and real time seconds
*Added termination for parent process exceeding real time of 2 seconds
*Added ability to determine how many nanoseconds had passed in child process

2/27/20
*Added check for primality in child process
*Added periodic check (in primality loop) for child process to terminate if 
taking longer than 1 millisecond based on nanoseconds
*Added shared memory array for child results
*Added output file
*Output results in all fashions to output file
*Added perror checks where needed
*Added check to keep concurrent process from exceeding 20
*Added all additional termination criteria including Ctrl-C termination
and subsequent output
*Tested different process setups using options
*Ensured shared memory deallocation in all cases
