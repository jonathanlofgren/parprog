CC = gcc           # C compiler
LD = gcc           # C linker
CFLAGS = -g -Wall  # Flags to be passed to the compiler
LDFLAGS =          # Flags for the linker
RM = /bin/rm -f    # Deletion program
OBJS = matrix.o main.o 
EXEC = matrix

all: matrix

matrix: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXEC) 

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

main.o: matrix.h main.c
	$(CC) $(CFLAGS) -c main.c 
clean:
	$(RM) $(EXEC) $(OBJS)
