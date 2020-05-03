CFLAGS=-Wall -g

SOURCE_MAIN=oss.c
SOURCE_SLAVE=user.c
OBJECT_MAIN=oss.o
OBJECT_SLAVE=user.o
EXE_MAIN=oss
EXE_SLAVE=user
all: $(EXE_MAIN) $(EXE_SLAVE)
$(EXE_MAIN): $(OBJECT_MAIN)
	gcc $(OBJECT_MAIN) -o $(EXE_MAIN)
$(EXE_SLAVE): $(OBJECT_SLAVE)
	gcc $(OBJECT_SLAVE) -o $(EXE_SLAVE) -lm
%.o: %.c 
	gcc -c $(CFLAGS) $*.c -o $*.o
.PHONY: clean
clean:
	rm *.o $(EXE_MAIN) $(EXE_SLAVE)

