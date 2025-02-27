CC = gcc
CFLAGS = -Wall -pthread -o airport_simulation

all: airport_simulation

airport_simulation: airport_simulation.c
	$(CC) $(CFLAGS) airport_simulation.c

clean:
	rm -f airport_simulation
