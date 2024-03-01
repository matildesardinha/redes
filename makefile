#RCI - Project

#make: compile all
#make clean: clean object files and executable


# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

PROGS=cor

all: $(PROGS)

cor: udp_functions.o main_test.c
	$(CC) $(CFLAGS) udp_functions.o main_test.c -o cor

udp_functions.o: udp_functions.c udp_functions.h 
	$(CC) $(CFLAGS) -c udp_functions.c

clean:
	rm -f $(PROGS) *.o