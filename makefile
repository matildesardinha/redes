#RCI - Project

#make: compile all
#make clean: clean object files and executable


# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

PROGS=cor

all: $(PROGS)

test: udp_functions.o main_test.c
	$(CC) $(CFLAGS) udp_functions.o main_test.c -o test

cor: check_usage.o udp_functions.o tcp_functions.o data_structures.o ring_message.o  commands.o  main.c
	$(CC) $(CFLAGS) check_usage.o udp_functions.o tcp_functions.o data_structures.o ring_message.o  commands.o  main.c -o cor

commands.o: commands.c commands.h data_structures.h udp_functions.h tcp_functions.h ring_message.h
	$(CC) $(CFLAGS) -c commands.c

ring_message.o: ring_message.c ring_message.h tcp_functions.h udp_functions.h data_structures.h
	$(CC) $(CFLAGS) -c ring_message.c

data_structures.o: data_structures.c data_structures.h udp_functions.h
	$(CC) $(CFLAGS) -c data_structures.c	

tcp_functions.o: tcp_functions.c tcp_functions.h 
	$(CC) $(CFLAGS) -c tcp_functions.c

udp_functions.o: udp_functions.c udp_functions.h 
	$(CC) $(CFLAGS) -c udp_functions.c

check_usage.o: check_usage.c check_usage.h 
	$(CC) $(CFLAGS) -c check_usage.c
clean:
	rm -f $(PROGS) *.o