# Makefile for Banker's Algorithm Project
# Created by James Grantham
# Creation Date: 10/9/2015
#

# Compiler currently in use
CC=g++

# Compiler flag options
CFLAGS=-c

# Loader flag options
LDFLAGS=

# Name of the program executable file
EXECUTABLE=shiftregister

all: $(EXECUTABLE)


$(EXECUTABLE): main.o
	$(CC) main.o $(LDFLAGS) -o $(EXECUTABLE)

main.o:
	$(CC) $(CFLAGS) shiftregister.cpp -o main.o

clean:
	rm *o $(EXECUTABLE)

