Option 3.

The FSA table is an int array named fsaTable located in scanner.c.
The driver is also in scanner.c and is named FADriver.

Program Description:
A scanner which breaks source code into meaninful symbols which the parser can work with.

Invocation:
Run make
The command to run is './scanner' or './scanner fileName'

It is important to note that if you use the first invocation and manually enter
strings to be sent to the scanner, any spaces between strings entered on the same line
will results in a newline in the 'output' file and thus will be represented by appropriate
line numbers when the testScanner prints results from the scanner.

Git Log:

3/16/20
* Edited main.c to coincide with P1
* Created files token.h, scanner.c, scanner.h, testScanner.c, testScanner.h
* Created Makefile

3/17/20
* Created FSA table
* Created token structure in token.h
* Linked token structure to scanner.c and testScanner.c

3/18/20
* Created scanner function in scanner.c
  - Tested by calling from testScanner.c
* Created testScanner function in testScanner.c
  - Tested loop to repeat call to scanner() until EOF token is returns from scanner function

3/19/20
* Created driver function
* Created filter function

3/21/20
* Created function to assign lookahead character "getNextChar()"
* Worked on driver function
* Workded on filter function

3/22/20
* Finished driver function
* Finished filter function
  - Added comment check
* Created lots of bugs

3/23/20
* Added check to identify keywords
* Fixed remaining bugs
* Tested against testing scenarios suggested in prompt
* Fixed more bugs
