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

***Comparison of Methods***
There are two ways you could approach the problem as I see it. 

Approach 1:

You avoid unexpected results in the first method by assuming results given to that method 
will always be a power of 2 and therefore even. This means that if your log summation gives 
you back an odd number of results or generally a number that isn't a power of 2 you may have an issue 
with your forking. This was the method I chose and my solution was to ensure that when I recalled method 
1 after doing the log summation from method 2 I kept the same number of integers I had originally. 
I achieved this by zero'ing out all the numbers in the shared memory array other than the results 
from method 2. In this case, overall, method 2 takes longer because you are doing the same amount 
of summations as the first method but also the few log summations as well.

Approach 2: 

You alter method 1 by taking the ceiling of n/2 to account for odd numbers. For this I assume you need 
to create a new shared memory array of size 'results', that is the number of integers returned from method 2,
OR perhaps simply ensure you only fork off the ceiling of n/2 processes when recalling method 1. In this 
case, method 2 should take less time than method one, because as Rakeem pointed out, even with the few log 
summations your method 2 will be summing fewer integers.

tldr: With the approach I took, method 2 takes longer because it does the same number of summations as method
1 but also a few more summations to get the log results.

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