# Makefile for Banker's Algorithm Project
# Created by James Grantham
# Creation Date: 10/9/2015
#

# Compiler currently in use
CC=g++ 

# Compiler flag options
CFLAGS=-c -std=c++11 -g

# Loader flag options
LDFLAGS=-pthread -lpthread

# Name of the program executable file
EXECUTABLE=bankerAlg

all: $(EXECUTABLE)


$(EXECUTABLE): main.o
	$(CC) main.o $(LDFLAGS) -o $(EXECUTABLE)

main.o:
	$(CC) $(CFLAGS) main.cpp

clean:
	rm *o bankerAlg

