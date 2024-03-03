#RCI - Project

#make: compile all
#make clean: clean object files and executable


# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g

PROGS=cor

all: $(PROGS)

cor: udp_functions.o tcp_functions.o ring_messages.o data_structures.o commands.o check_usage.o main.c
	$(CC) $(CFLAGS) udp_functions.o tcp_functions.o ring_messages.o data_structures.o commands.o check_usage.o main.c -o cor

check_usage.o: check_usage.c check_usage.h 
	$(CC) $(CFLAGS) -c check_usage.c

commands.o: commands.c commands.h data_structures.h udp_functions.h tcp_functions.h ring_messages.h
	$(CC) $(CFLAGS) -c commands.c

data_structures.o: data_structures.c data_structures.h udp_functions.h
	$(CC) $(CFLAGS) -c data_structures.c

ring_messages.o: ring_messages.c ring_messages.h tcp_functions.h udp_functions.h data_structures.h
	$(CC) $(CFLAGS) -c ring_messages.c

tcp_functions.o: tcp_functions.c tcp_functions.h 
	$(CC) $(CFLAGS) -c tcp_functions.c

udp_functions.o: udp_functions.c udp_functions.h 
	$(CC) $(CFLAGS) -c udp_functions.c

clean:
	rm -f $(PROGS) *.o