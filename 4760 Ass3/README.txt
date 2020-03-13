Git log:
3/10/20
* Created git repo
* Created input file generator
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