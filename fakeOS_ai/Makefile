CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = shell

ALL: shell.o str.o libkernel.a
	$(CC) ${CFLAGS} shell.o str.o -o shell -L. -lkernel

libkernel.a: kernel.h syscalls.h syscalls.o process.o mem.o network.o
	ar rcs libkernel.a syscalls.o process.o mem.o network.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -f shell libkernel.a *.o

# .PHONY: clean