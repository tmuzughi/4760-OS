# 4760-OS

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
